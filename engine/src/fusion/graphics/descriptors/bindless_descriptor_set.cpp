#include "bindless_descriptor_set.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

static const uint32_t MAX_BINDLESS_RESOURCES = 1024;

BindlessDescriptorSet::BindlessDescriptorSet(uint32_t binding) : binding{binding} {
    createDescriptorLayout();
    createDescriptorPool();
    createDescriptors();
}

BindlessDescriptorSet::~BindlessDescriptorSet() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

    VK_CHECK(vkFreeDescriptorSets(logicalDevice, descriptorPool, 1, &descriptorSet));
    vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
}

void BindlessDescriptorSet::createDescriptorLayout() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorSetLayoutBinding.descriptorCount = MAX_BINDLESS_RESOURCES;
    descriptorSetLayoutBinding.binding = binding;
    descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;
    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorBindingFlags bindlessFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT, nullptr };
    extendedInfo.bindingCount = 1;
    extendedInfo.pBindingFlags = &bindlessFlags;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;
    descriptorSetLayoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
    descriptorSetLayoutCreateInfo.pNext = &extendedInfo;
    VK_CHECK(vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));
}

void BindlessDescriptorSet::createDescriptorPool() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

    // Create bindless descriptor pool
    std::vector<VkDescriptorPoolSize> descriptorPools = {
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_BINDLESS_RESOURCES },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, MAX_BINDLESS_RESOURCES },
    };

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
    descriptorPoolCreateInfo.maxSets = MAX_BINDLESS_RESOURCES * static_cast<uint32_t>(descriptorPools.size());
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();
    VK_CHECK(vkCreateDescriptorPool(logicalDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool));
}

void BindlessDescriptorSet::createDescriptors() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT countInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT };
    uint32_t maxBinding = MAX_BINDLESS_RESOURCES - 1;
    countInfo.descriptorSetCount = 1;
    countInfo.pDescriptorCounts = &maxBinding; // This number is the max allocatable count
    countInfo.pNext = &countInfo;

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
    descriptorSetAllocateInfo.pNext = &countInfo;
    VK_CHECK(vkAllocateDescriptorSets(logicalDevice, &descriptorSetAllocateInfo, &descriptorSet));
}

