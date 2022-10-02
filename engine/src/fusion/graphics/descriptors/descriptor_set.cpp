#include "descriptor_set.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"

using namespace fe;

// Indexed Descriptor sets
static const uint32_t BINDLESS_RESOURCES = MAX_BINDLESS_RESOURCES - 1;
static VkDescriptorSetVariableDescriptorCountAllocateInfoEXT variableDescriptorCountAllocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT, nullptr, 1, &BINDLESS_RESOURCES };

DescriptorSet::DescriptorSet(const Pipeline& pipeline) {
    if (pipeline.isIndexedDescriptors())
        Graphics::Get()->getIndexedDescriptorAllocator().allocateDescriptor(pipeline.getDescriptorSetLayout(), descriptorSet, &variableDescriptorCountAllocInfo);
    else
        Graphics::Get()->getDescriptorAllocator().allocateDescriptor(pipeline.getDescriptorSetLayout(), descriptorSet);
}

DescriptorSet::~DescriptorSet() {
    // TODO: Descriptor allocator should be clean up ?
    //const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
	//VK_CHECK(vkFreeDescriptorSets(logicalDevice, descriptorPool, 1, &descriptorSet));
}

void DescriptorSet::updateDescriptor(std::span<const VkWriteDescriptorSet> descriptorWrites) {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
	vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void DescriptorSet::bindDescriptor(const CommandBuffer& commandBuffer, const Pipeline& pipeline) const {
    vkCmdBindDescriptorSets(commandBuffer, pipeline.getPipelineBindPoint(), pipeline.getPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);
}
