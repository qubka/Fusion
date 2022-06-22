#include "descriptor_set.hpp"

#include "fusion/renderer/graphics.hpp"

using namespace fe;

DescriptorSet::DescriptorSet(const Pipeline& pipeline)
    : pipelineLayout{pipeline.getPipelineLayout()}
    , pipelineBindPoint{pipeline.getPipelineBindPoint()}
    , descriptorPool{pipeline.getDescriptorPool()}
{
	auto logicalDevice = Graphics::Get()->getLogicalDevice();

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &pipeline.getDescriptorSetLayout();
	Graphics::CheckVk(vkAllocateDescriptorSets(*logicalDevice, &descriptorSetAllocateInfo, &descriptorSet));
}

DescriptorSet::~DescriptorSet() {
	auto logicalDevice = Graphics::Get()->getLogicalDevice();

	Graphics::CheckVk(vkFreeDescriptorSets(*logicalDevice, descriptorPool, 1, &descriptorSet));
}

void DescriptorSet::update(const std::vector<VkWriteDescriptorSet>& descriptorWrites) {
	auto logicalDevice = Graphics::Get()->getLogicalDevice();

	vkUpdateDescriptorSets(*logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void DescriptorSet::bindDescriptor(const CommandBuffer& commandBuffer) const {
	vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}
