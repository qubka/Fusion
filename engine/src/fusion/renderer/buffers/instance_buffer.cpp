#include "instance_buffer.hpp"

using namespace fe;

InstanceBuffer::InstanceBuffer(VkDeviceSize size) : Buffer{size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT} {
}

void InstanceBuffer::update(const CommandBuffer& commandBuffer, const void* newData) {
	void* data;
	mapMemory(&data);
	memcpy(data, newData, static_cast<size_t>(size));
	unmapMemory();
}