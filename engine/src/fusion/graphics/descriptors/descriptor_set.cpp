#include "descriptor_set.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"

using namespace fe;

DescriptorSet::DescriptorSet(const Pipeline& pipeline)
        : pipelineLayout{pipeline.getPipelineLayout()}
        , pipelineBindPoint{pipeline.getPipelineBindPoint()} {
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

void DescriptorSet::bindDescriptor(const CommandBuffer& commandBuffer) const {
	vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}
