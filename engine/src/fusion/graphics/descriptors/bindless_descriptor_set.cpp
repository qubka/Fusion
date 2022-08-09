#include "bindless_descriptor_set.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

static const uint32_t MAX_BINDLESS_RESOURCES = 1024; // Same as pool size
static const uint32_t MAX_BINDING = MAX_BINDLESS_RESOURCES - 1;

// TODO: Find workaround for pNext chain
static VkDescriptorBindingFlags bindlessFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
static VkDescriptorSetLayoutBindingFlagsCreateInfoEXT descriptorSetLayoutBindingFlagsCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT, nullptr, 1, &bindlessFlags };
static VkDescriptorSetVariableDescriptorCountAllocateInfoEXT descriptorSetVariableDescriptorCountAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT, nullptr, 1, &MAX_BINDING };

BindlessDescriptorSet::BindlessDescriptorSet(uint32_t binding) {
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorSetLayoutBinding.descriptorCount = MAX_BINDLESS_RESOURCES;
    descriptorSetLayoutBinding.binding = binding;
    descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptorSetLayoutCreateInfo.bindingCount = 1;
    descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;
    descriptorSetLayoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
    descriptorSetLayoutCreateInfo.pNext = &descriptorSetLayoutBindingFlagsCreateInfo;

    descriptorSetLayout = Graphics::Get()->getDescriptorLayoutCache().createDescriptorLayout(descriptorSetLayoutCreateInfo);

    Graphics::Get()->getBindlessDescriptorAllocator().allocateDescriptor(descriptorSetLayout, descriptorSet, &descriptorSetVariableDescriptorCountAllocateInfo);
}

BindlessDescriptorSet::~BindlessDescriptorSet() {
    // TODO: Descriptor allocator should be clean up ?
    //const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    //VK_CHECK(vkFreeDescriptorSets(logicalDevice, descriptorPool, 1, &descriptorSet));
}