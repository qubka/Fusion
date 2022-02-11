#include "Descriptors.hpp"

using namespace Fusion;

// *************** Descriptor Pool Builder *********************

DescriptorPool::Builder& DescriptorPool::Builder::addPoolSize(vk::DescriptorType descriptorType, uint32_t count) {
    poolSizes.emplace_back(descriptorType, count);
    return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::setPoolFlags(vk::DescriptorPoolCreateFlags flags) {
    poolFlags = flags;
    return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::setMaxSets(uint32_t count) {
    maxSets = count;
    return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
    return std::make_unique<DescriptorPool>(*this);
}

// *************** Descriptor Pool *********************

DescriptorPool::DescriptorPool(const Builder& builder) : device{builder.device} {
    vk::DescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(builder.poolSizes.size());
    descriptorPoolInfo.pPoolSizes = builder.poolSizes.data();
    descriptorPoolInfo.maxSets = builder.maxSets;
    descriptorPoolInfo.flags = builder.poolFlags;

    auto result = device.getLogical().createDescriptorPool(&descriptorPoolInfo, nullptr, &descriptorPool);
    FS_CORE_ASSERT(result == vk::Result::eSuccess, "failed to create descriptor pool!");
}

DescriptorPool::~DescriptorPool() {
    device.getLogical().destroyDescriptorPool(descriptorPool, nullptr);
}

bool DescriptorPool::allocateDescriptor(const vk::DescriptorSetLayout& setLayout, vk::DescriptorSet& descriptor) const {
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.pSetLayouts = &setLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (device.getLogical().allocateDescriptorSets(&allocInfo, &descriptor) != vk::Result::eSuccess) {
        FS_LOG_CORE_ERROR("pools fills up!");
        return false;
    }

    return true;
}

void DescriptorPool::freeDescriptors(std::vector<vk::DescriptorSet>& descriptorSets) const {
    device.getLogical().freeDescriptorSets(descriptorPool, static_cast<uint32_t>(descriptorSets.size()),descriptorSets.data());
}

void DescriptorPool::resetPool() {
    device.getLogical().resetDescriptorPool(descriptorPool, vk::DescriptorPoolResetFlags());
}

// *************** Descriptor Set Layout Builder *********************

DescriptorLayout::Builder& DescriptorLayout::Builder::addBinding(uint32_t binding, vk::DescriptorType descriptorType, vk::ShaderStageFlags stageFlags, uint32_t count) {
    FS_CORE_ASSERT(bindings.count(binding) == 0, "binding already in use");
    vk::DescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    bindings[binding] = layoutBinding;
    return *this;
}

std::unique_ptr<DescriptorLayout> DescriptorLayout::Builder::build() const {
    return std::make_unique<DescriptorLayout>(*this);
}

// *************** Descriptor Set Layout *********************

DescriptorLayout::DescriptorLayout(const Builder& builder) : device{builder.device}, bindings{builder.bindings} {
    std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings;
    setLayoutBindings.reserve(bindings.size());

    for (const auto& b : bindings) {
        setLayoutBindings.push_back(b.second);
    }

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    auto result = device.getLogical().createDescriptorSetLayout(&descriptorSetLayoutInfo, nullptr, &descriptorSetLayout);
    FS_CORE_ASSERT(result == vk::Result::eSuccess, "failed to create descriptor set layout!");
}

DescriptorLayout::~DescriptorLayout() {
    device.getLogical().destroyDescriptorSetLayout(descriptorSetLayout, nullptr);
}

// *************** Descriptor Writer *********************

DescriptorWriter::DescriptorWriter(DescriptorLayout& layout, DescriptorPool& pool) : layout{layout}, pool{pool} {
}

DescriptorWriter& DescriptorWriter::writeBuffer(uint32_t binding, const vk::DescriptorBufferInfo& bufferInfo) {
    FS_CORE_ASSERT(layout.bindings.count(binding) == 1, "layout does not contain specified binding");
    const auto& bindingDescription = layout.bindings[binding];
    FS_CORE_ASSERT(bindingDescription.descriptorCount == 1, "binding single descriptor info, but binding expects multiple");

    vk::WriteDescriptorSet write{};
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = &bufferInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

DescriptorWriter& DescriptorWriter::writeImage(uint32_t binding, const vk::DescriptorImageInfo& imageInfo) {
    FS_CORE_ASSERT(layout.bindings.count(binding) == 1, "layout does not contain specified binding");
    const auto& bindingDescription = layout.bindings[binding];
    FS_CORE_ASSERT(bindingDescription.descriptorCount == 1, "binding single descriptor info, but binding expects multiple");

    vk::WriteDescriptorSet write{};
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = &imageInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    return *this;
}

bool DescriptorWriter::build(vk::DescriptorSet& set) {
    bool success = pool.allocateDescriptor(layout.getDescriptorSetLayout(), set);
    if (!success) {
        return false;
    }
    overwrite(set);
    return true;
}

void DescriptorWriter::overwrite(vk::DescriptorSet& set) {
    for (auto& write: writes) {
        write.dstSet = set;
    }

    pool.device.getLogical().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}