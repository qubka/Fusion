#include "Descriptors.hpp"

using namespace Fusion;

DescriptorAllocator::DescriptorAllocator(Vulkan& vulkan) : vulkan{vulkan} {
}

DescriptorAllocator::~DescriptorAllocator() {
    //delete every pool held
    for (const auto& p : freePools) {
        vulkan.getDevice().destroyDescriptorPool(p, nullptr);
    }
    for (const auto& p : usedPools) {
        vulkan.getDevice().destroyDescriptorPool(p, nullptr);
    }
}

vk::DescriptorPool DescriptorAllocator::grabPool() {
    if (!freePools.empty()) {
        //grab pool from the back of the vector and remove it from there.
        vk::DescriptorPool pool = freePools.back();
        freePools.pop_back();
        return pool;
    } else {
        //no pools availible, so create a new one
        return createPool(INITIAL_POOL_SIZE, {});
    }
}

vk::DescriptorPool DescriptorAllocator::createPool(uint32_t count, vk::DescriptorPoolCreateFlags flags) const {
    std::vector<vk::DescriptorPoolSize> sizes;
    sizes.reserve(descriptorSizes.sizes.size());

    for (auto sz : descriptorSizes.sizes) {
        sizes.emplace_back(sz.first, static_cast<uint32_t>(sz.second * count));
    }

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.flags = flags;
    poolInfo.maxSets = count;
    poolInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
    poolInfo.pPoolSizes = sizes.data();

    vk::DescriptorPool descriptorPool;

    auto result = vulkan.getDevice().createDescriptorPool(&poolInfo, nullptr, &descriptorPool);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create descriptor pool!");

    return descriptorPool;
}

bool DescriptorAllocator::allocateDescriptor(const vk::DescriptorSetLayout& layout, vk::DescriptorSet& set) {
    //initialize the currentPool handle if it's null
    if (!currentPool) {
        currentPool = grabPool();
        usedPools.push_back(currentPool);
    }

    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.pNext = nullptr;
    allocInfo.pSetLayouts = &layout;
    allocInfo.descriptorPool = currentPool;
    allocInfo.descriptorSetCount = 1;

    //try to allocate the descriptor set
    auto result = vulkan.getDevice().allocateDescriptorSets(&allocInfo, &set);

    switch (result) {
        case vk::Result::eSuccess:
            //all good, return
            return true;
        case vk::Result::eErrorFragmentedPool:
        case vk::Result::eErrorOutOfPoolMemory:
            //allocate a new pool and retry
            currentPool = grabPool();
            usedPools.push_back(currentPool);

            result = vulkan.getDevice().allocateDescriptorSets(&allocInfo, &set);

            //if it still fails then we have big issues
            if (result == vk::Result::eSuccess) {
                return true;
            }
            break;
        default:
            //unrecoverable error
            return false;
    }

    return false;
}

void DescriptorAllocator::resetPools() {
    //reset all used pools and add them to the free pools
    for (const auto& p : usedPools) {
        vulkan.getDevice().resetDescriptorPool(p, {});
        freePools.push_back(p);
    }

    //clear the used pools, since we've put them all in the free pools
    usedPools.clear();

    //reset the current pool handle back to null
    currentPool = nullptr;
}

void DescriptorAllocator::updateDescriptor(std::vector<vk::WriteDescriptorSet>& writes) const {
    vulkan.getDevice().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

DescriptorLayoutCache::DescriptorLayoutCache(Vulkan& vulkan) : vulkan{vulkan} {
}

DescriptorLayoutCache::~DescriptorLayoutCache() {
    //delete every descriptor layout held
    for (const auto& [info, layout] : layoutCache) {
        vulkan.getDevice().destroyDescriptorSetLayout(layout, nullptr);
    }
}

vk::DescriptorSetLayout DescriptorLayoutCache::createDescriptorLayout(vk::DescriptorSetLayoutCreateInfo& info) {
    DescriptorLayoutInfo layoutInfo;
    layoutInfo.bindings.reserve(info.bindingCount);
    bool isSorted = true;
    int lastBinding = -1;

    //copy from the direct info struct into our own one
    for (int i = 0; i < info.bindingCount; i++) {
        layoutInfo.bindings.push_back(info.pBindings[i]);

        //check that the bindings are in strict increasing order
        int binding = static_cast<int>(info.pBindings[i].binding);
        if (binding > lastBinding) {
            lastBinding = binding;
        } else {
            isSorted = false;
        }
    }
    //sort the bindings if they aren't in order
    if (!isSorted) {
        std::sort(layoutInfo.bindings.begin(), layoutInfo.bindings.end(),
                  [](const vk::DescriptorSetLayoutBinding& a, const vk::DescriptorSetLayoutBinding& b) {
              return a.binding < b.binding;
          });
    }

    //try to grab from cache
    if (auto it { layoutCache.find(layoutInfo)}; it != layoutCache.end()) {
        return (*it).second;
    } else {
        //create a new one (not found)
        vk::DescriptorSetLayout layout;

        auto result = vulkan.getDevice().createDescriptorSetLayout(&info, nullptr, &layout);
        FE_ASSERT(result == vk::Result::eSuccess && "failed to create descriptor set layout!");

        //add to cache
        layoutCache[layoutInfo] = layout;
        return layout;
    }
}

bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const{
    if (other.bindings.size() != bindings.size()){
        return false;
    } else {
        //compare each of the bindings is the same. Bindings are sorted so they will match
        for (int i = 0; i < bindings.size(); i++) {
            if (other.bindings[i].binding != bindings[i].binding) {
                return false;
            }
            if (other.bindings[i].descriptorType != bindings[i].descriptorType) {
                return false;
            }
            if (other.bindings[i].descriptorCount != bindings[i].descriptorCount) {
                return false;
            }
            if (other.bindings[i].stageFlags != bindings[i].stageFlags) {
                return false;
            }
        }
        return true;
    }
}

size_t DescriptorLayoutCache::DescriptorLayoutInfo::hash() const{
    size_t seed = std::hash<size_t>()(bindings.size());

    for (const auto& b : bindings) {
        //pack the binding data
        hashCombine(seed, b.binding, static_cast<uint32_t>(b.descriptorType), b.descriptorCount, static_cast<uint32_t>(b.stageFlags));
    }

    return seed;
}

DescriptorBuilder::DescriptorBuilder(DescriptorLayoutCache& cache, DescriptorAllocator& allocator) : cache{cache}, allocator{allocator} {
}

DescriptorBuilder::~DescriptorBuilder() {
}

DescriptorBuilder& DescriptorBuilder::bindBuffer(uint32_t binding, vk::DescriptorBufferInfo* bufferInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags) {
    //create the descriptor binding for the layout
    vk::DescriptorSetLayoutBinding newBinding{};
    newBinding.descriptorCount = 1;
    newBinding.descriptorType = type;
    newBinding.pImmutableSamplers = nullptr;
    newBinding.stageFlags = stageFlags;
    newBinding.binding = binding;

    bindings.push_back(newBinding);

    //create the descriptor write
    vk::WriteDescriptorSet newWrite{};
    newWrite.pNext = nullptr;
    newWrite.descriptorCount = 1;
    newWrite.descriptorType = type;
    newWrite.pBufferInfo = bufferInfo;
    newWrite.dstBinding = binding;

    writes.push_back(newWrite);
    return *this;
}

DescriptorBuilder& DescriptorBuilder::bindImage(uint32_t binding, vk::DescriptorImageInfo* imageInfo, vk::DescriptorType type, vk::ShaderStageFlags stageFlags) {
    //create the descriptor binding for the layout
    vk::DescriptorSetLayoutBinding newBinding{};
    newBinding.descriptorCount = 1;
    newBinding.descriptorType = type;
    newBinding.pImmutableSamplers = nullptr;
    newBinding.stageFlags = stageFlags;
    newBinding.binding = binding;

    bindings.push_back(newBinding);

    //create the descriptor write
    vk::WriteDescriptorSet newWrite{};
    newWrite.pNext = nullptr;
    newWrite.descriptorCount = 1;
    newWrite.descriptorType = type;
    newWrite.pImageInfo = imageInfo;
    newWrite.dstBinding = binding;

    writes.push_back(newWrite);
    return *this;
}

bool DescriptorBuilder::build(vk::DescriptorSet& set, vk::DescriptorSetLayout& layout) {
    //build layout first
    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.pNext = nullptr;
    layoutInfo.pBindings = bindings.data();
    layoutInfo.bindingCount = bindings.size();

    layout = cache.createDescriptorLayout(layoutInfo);

    //allocate descriptor
    bool success = allocator.allocateDescriptor(layout, set);
    if (!success) return false;

    //write descriptor
    for (auto& w : writes) {
        w.dstSet = set;
    }

    allocator.updateDescriptor(writes);
    return true;
}

bool DescriptorBuilder::build(vk::DescriptorSet& set) {
    //build layout first
    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.pNext = nullptr;
    layoutInfo.pBindings = bindings.data();
    layoutInfo.bindingCount = bindings.size();

    auto layout = cache.createDescriptorLayout(layoutInfo);

    //allocate descriptor
    bool success = allocator.allocateDescriptor(layout, set);
    if (!success) return false;

    //write descriptor
    for (auto& w : writes) {
        w.dstSet = set;
    }

    allocator.updateDescriptor(writes);
    return true;
}