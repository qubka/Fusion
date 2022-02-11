#include "AllocatedBuffer.hpp"

using namespace Fusion;

AllocatedBuffer::AllocatedBuffer(
        Device& device,
        vk::DeviceSize instanceSize,
        uint32_t instanceCount,
        vk::BufferUsageFlags usageFlags,
        vk::MemoryPropertyFlags memoryPropertyFlags,
        vk::DeviceSize minOffsetAlignment
    ) : device{device},
        instanceSize{instanceSize},
        instanceCount{instanceCount},
        usageFlags{usageFlags},
        memoryPropertyFlags{memoryPropertyFlags} {
    alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    bufferSize = alignmentSize * instanceCount;
    device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
}

AllocatedBuffer::~AllocatedBuffer() {
    unmap();
    device.getLogical().destroyBuffer(buffer, nullptr);
    device.getLogical().freeMemory(memory, nullptr);
}

/**
 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
 *
 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
 * buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the buffer mapping call
 */
void AllocatedBuffer::map(vk::DeviceSize size, vk::DeviceSize offset) {
    FS_CORE_ASSERT(buffer && memory, "Called map on buffer before create");
    auto result = device.getLogical().mapMemory(memory, offset, size, vk::MemoryMapFlagBits(), &mapped);
    FS_CORE_ASSERT(result == vk::Result::eSuccess, "failed to map memory on the device!");
}

/**
 * Unmap a mapped memory range
 *
 * @note Does not return a result as vkUnmapMemory can't fail
 */
void AllocatedBuffer::unmap() {
    if (mapped) {
        device.getLogical().unmapMemory(memory);
        mapped = nullptr;
    }
}

/**
 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
 *
 * @param data Pointer to the data to copy
 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
 * range.
 * @param offset (Optional) Byte offset from beginning of mapped region
 *
 */
void AllocatedBuffer::writeToBuffer(void* data, vk::DeviceSize size, vk::DeviceSize offset) {
    FS_CORE_ASSERT(mapped, "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
        memcpy(mapped, data, bufferSize);
    } else {
        auto* memOffset = (int8_t*) mapped;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

/**
 * Flush a memory range of the buffer to make it visible to the device
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
 * complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the flush call
 */
vk::Result AllocatedBuffer::flush(vk::DeviceSize size, vk::DeviceSize offset) {
    vk::MappedMemoryRange mappedRange{ memory, offset, size };
    return device.getLogical().flushMappedMemoryRanges(1, &mappedRange);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
 * the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the invalidate call
 */
vk::Result AllocatedBuffer::invalidate(vk::DeviceSize size, vk::DeviceSize offset) {
    vk::MappedMemoryRange mappedRange{ memory, offset, size };
    return device.getLogical().invalidateMappedMemoryRanges(1, &mappedRange);
}

/**
 * Create a buffer info descriptor
 *
 * @param size (Optional) Size of the memory range of the descriptor
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkDescriptorBufferInfo of specified offset and range
 */
vk::DescriptorBufferInfo AllocatedBuffer::descriptorInfo(vk::DeviceSize size, vk::DeviceSize offset) {
    return vk::DescriptorBufferInfo{ buffer, offset, size };
}

/**
 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
 *
 * @param data Pointer to the data to copy
 * @param index Used in offset calculation
 *
 */
void AllocatedBuffer::writeToIndex(void* data, int index) {
    writeToBuffer(data, instanceSize, index * alignmentSize);
}

/**
 *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
 *
 * @param index Used in offset calculation
 *
 */
vk::Result AllocatedBuffer::flushIndex(int index) {
    return flush(alignmentSize, index * alignmentSize);
}

/**
 * Create a buffer info descriptor
 *
 * @param index Specifies the region given by index * alignmentSize
 *
 * @return VkDescriptorBufferInfo for instance at index
 */
vk::DescriptorBufferInfo AllocatedBuffer::descriptorInfoForIndex(int index) {
    return descriptorInfo(alignmentSize, index * alignmentSize);
}

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param index Specifies the region to invalidate: index * alignmentSize
 *
 * @return VkResult of the invalidate call
 */
vk::Result AllocatedBuffer::invalidateIndex(int index) {
    return invalidate(alignmentSize, index * alignmentSize);
}

/**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
vk::DeviceSize AllocatedBuffer::getAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}
