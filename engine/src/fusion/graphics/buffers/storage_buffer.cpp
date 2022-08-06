#include "storage_buffer.hpp"

using namespace fe;

StorageBuffer::StorageBuffer(VkDeviceSize size, const void *data) :
	Buffer{size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, data} {
}

void StorageBuffer::update(const void* newData) {
    map();
    copy(newData, size);
    unmap();
}

WriteDescriptorSet StorageBuffer::getWriteDescriptor(uint32_t binding, VkDescriptorType descriptorType, const std::optional<OffsetSize>& offsetSize) const {
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = buffer;
	if (offsetSize) {
		bufferInfo.offset = offsetSize->getOffset();
		bufferInfo.range = offsetSize->getSize();
	} else {
        bufferInfo.offset = 0;
        bufferInfo.range = size;
    }

	VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptorWrite.dstSet = VK_NULL_HANDLE; // Will be set in the descriptor handler.
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = descriptorType;
	//descriptorWrite.pBufferInfo = &bufferInfo;
	return {descriptorWrite, bufferInfo};
}

VkDescriptorSetLayoutBinding StorageBuffer::GetDescriptorSetLayout(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stage, uint32_t count) {
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
	descriptorSetLayoutBinding.binding = binding;
	descriptorSetLayoutBinding.descriptorType = descriptorType;
	descriptorSetLayoutBinding.descriptorCount = count;
	descriptorSetLayoutBinding.stageFlags = stage;
	descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
	return descriptorSetLayoutBinding;
}