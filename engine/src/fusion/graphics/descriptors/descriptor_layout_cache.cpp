#include "descriptor_layout_cache.hpp"

using namespace fe;

DescriptorLayoutCache::DescriptorLayoutCache(VkDevice device) : device{device} {

}

DescriptorLayoutCache::~DescriptorLayoutCache() {
    // Delete every descriptor layout held
    for (const auto& [info, layout] : layoutCache) {
        vkDestroyDescriptorSetLayout(device, layout, nullptr);
    }
}

VkDescriptorSetLayout DescriptorLayoutCache::createDescriptorLayout(const VkDescriptorSetLayoutCreateInfo& info) const {
    DescriptorLayoutInfo layoutInfo = {};
    layoutInfo.flags = info.flags;
    layoutInfo.bindings.reserve(info.bindingCount);

    bool isSorted = true;
    uint32_t lastBinding = UINT32_MAX;

    // Copy from the direct info struct into our own one
    for (const auto& [i, binding] : enumerate(std::span(info.pBindings, info.bindingCount))) {
        layoutInfo.bindings.push_back(binding);

        // Check that the bindings are in strict increasing order
        if (lastBinding == UINT32_MAX || binding.binding > lastBinding) {
            lastBinding = binding.binding;
        } else{
            isSorted = false;
        }
    }

    // Sort the bindings if they aren't in order
    if (!isSorted){
        std::sort(layoutInfo.bindings.begin(), layoutInfo.bindings.end(), [](const VkDescriptorSetLayoutBinding& a, const VkDescriptorSetLayoutBinding& b){
            return a.binding < b.binding;
        });
    }

    // Try to grab from cache
    if (auto it = layoutCache.find(layoutInfo); it != layoutCache.end()) {
        LOG_INFO << "Find \"VkDescriptorSetLayout\" in cache";
        return it->second;
    } else {
        // Create a new one (not found)
        VkDescriptorSetLayout descriptorSetLayout;
        VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &descriptorSetLayout));

        // Add to cache
        layoutCache[layoutInfo] = descriptorSetLayout;
        return descriptorSetLayout;
    }
}

bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& rhs) const {
    if (rhs.flags != flags || rhs.bindings.size() != bindings.size()) {
        return false;
    } else {
        // Compare each of the bindings is the same. Bindings are sorted so they will match
        for (const auto& [i, binding] : enumerate(bindings)) {
            if (rhs.bindings[i] != binding) {
                return false;
            }
        }
        return true;
    }
}

size_t DescriptorLayoutCache::DescriptorLayoutHash::operator()(const DescriptorLayoutCache::DescriptorLayoutInfo& i) const {
    size_t seed = 0;
    hash_combine(seed, i.flags, i.bindings.size());
    for (const auto& b : i.bindings) {
        hash_combine(seed, b.binding, b.descriptorType, b.descriptorCount, b.stageFlags);
    }
    return seed;
}
