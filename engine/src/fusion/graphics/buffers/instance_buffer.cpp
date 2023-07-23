#include "instance_buffer.h"

using namespace fe;

InstanceBuffer::InstanceBuffer(VkDeviceSize size) : Buffer{size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT} {
}

void InstanceBuffer::update(const CommandBuffer& commandBuffer, const void* newData) {
	map();
    copy(newData, size);
	unmap();
}