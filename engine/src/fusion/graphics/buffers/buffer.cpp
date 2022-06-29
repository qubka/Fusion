#include "buffer.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"

using namespace fe;

Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, const void* data)
    : logicalDevice{Graphics::Get()->getLogicalDevice()}
    , size{size} {
    const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();

    auto graphicsFamily = physicalDevice.getGraphicsFamily();
    auto presentFamily = physicalDevice.getPresentFamily();
    auto computeFamily = physicalDevice.getComputeFamily();

    std::array queueFamily = {graphicsFamily, presentFamily, computeFamily};

    // Create the buffer handle.
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
    bufferCreateInfo.pQueueFamilyIndices = queueFamily.data();
    VK_CHECK(vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, &buffer));

    // Create the memory backing up the buffer handle.
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memoryRequirements);

    // Create the memory backing up the buffer handle
    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    // Find a memory type index that fits the properties of the buffer
    memoryAllocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties);
    // If the buffer has VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT set we also need to enable the appropriate flag during allocation
    if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
        VkMemoryAllocateFlagsInfoKHR allocFlagsInfo = {};
        allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
        allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
        memoryAllocateInfo.pNext = &allocFlagsInfo;
    }
    VK_CHECK(vkAllocateMemory(logicalDevice, &memoryAllocateInfo, nullptr, &memory));

    // If a pointer to the buffer data has been passed, map the buffer and copy over the data.
    if (data) {
        map(size);
        copy(data);

        // If host coherency hasn't been requested, do a manual flush to make writes visible.
        if ((properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
            flush(size);
        }

        unmap();
    }
    
    // Attach the memory to the buffer object.
    VK_CHECK(vkBindBufferMemory(logicalDevice, buffer, memory, 0));
}

Buffer::~Buffer() {
    unmap();
    vkDestroyBuffer(logicalDevice, buffer, nullptr);
    vkFreeMemory(logicalDevice, memory, nullptr);
}

void Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    VK_CHECK(vkFlushMappedMemoryRanges(logicalDevice, 1, &mappedRange));
}

void Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    VK_CHECK(vkInvalidateMappedMemoryRanges(logicalDevice, 1, &mappedRange));
}

void Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
    assert(buffer && memory && "Called map on buffer before create");
    VK_CHECK(vkMapMemory(logicalDevice, memory, offset, size, 0, &mapped));
}

void Buffer::unmap() {
    if (mapped) {
        vkUnmapMemory(logicalDevice, memory);
        mapped = nullptr;
    }
}

void Buffer::copy(const void* data, VkDeviceSize size, VkDeviceSize offset) {
    assert(mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
        memcpy(mapped, data, this->size);
    } else {
        memcpy(reinterpret_cast<int8_t*>(mapped) + offset, data, size);
    }
}

int Buffer::compare(const void* data, VkDeviceSize size, VkDeviceSize offset) {
    assert(mapped && "Cannot compare to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
        return memcmp(mapped, data, this->size);
    } else {
        return memcmp(reinterpret_cast<int8_t*>(mapped) + offset, data, size);
    }
}

VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorBufferInfo{ buffer, offset, size, };
}

/*void Buffer::writeToIndex(const void* data, int index) {
    writeToBuffer(data, instanceSize, index * alignmentSize);
}

void Buffer::flushIndex(int index) {
    flush(alignmentSize, index * alignmentSize);
}

VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index) {
    return descriptorInfo(alignmentSize, index * alignmentSize);
}

void Buffer::invalidateIndex(int index) {
    invalidate(alignmentSize, index * alignmentSize);
}*/

VkDeviceSize Buffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

uint32_t Buffer::FindMemoryType(uint32_t typeFilter, const VkMemoryPropertyFlags& requiredProperties) {
    const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();
    auto memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        uint32_t memoryTypeBits = 1 << i;

        if (typeFilter & memoryTypeBits && (memoryProperties.memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find a valid memory type for buffer");
}

void Buffer::InsertBufferMemoryBarrier(const CommandBuffer& commandBuffer, const VkBuffer & buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
                                       VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDeviceSize offset, VkDeviceSize size) {
    VkBufferMemoryBarrier bufferMemoryBarrier = {};
    bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    bufferMemoryBarrier.srcAccessMask = srcAccessMask;
    bufferMemoryBarrier.dstAccessMask = dstAccessMask;
    bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.buffer = buffer;
    bufferMemoryBarrier.offset = offset;
    bufferMemoryBarrier.size = size;
    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
}