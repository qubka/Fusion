#include "buffer.h"

#include "fusion/graphics/graphics.h"
#include "fusion/graphics/commands/command_buffer.h"

using namespace fe;

Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, const void* data)
        : logicalDevice{Graphics::Get()->getLogicalDevice()}
        , size{size}
        , usageFlags{usage}
        , memoryPropertyFlags{properties} {
    const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();

    auto graphicsFamily = physicalDevice.getGraphicsFamily();
    auto presentFamily = physicalDevice.getPresentFamily();
    auto computeFamily = physicalDevice.getComputeFamily();

    std::array queueFamily = {graphicsFamily, presentFamily, computeFamily};

    // Create the buffer handle
    VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
    bufferCreateInfo.pQueueFamilyIndices = queueFamily.data();
    VK_CHECK(vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, &buffer));

    // Create the memory backing up the buffer handle
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memoryRequirements);

    // Create the memory backing up the buffer handle
    VkMemoryAllocateInfo memoryAllocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    // Find a memory type index that fits the properties of the buffer
    memoryAllocateInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties);
    // If the buffer has VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT set we also need to enable the appropriate flag during allocation
    if (usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) {
        VkMemoryAllocateFlagsInfoKHR allocFlagsInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR };
        allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
        memoryAllocateInfo.pNext = &allocFlagsInfo;
    }
    VK_CHECK(vkAllocateMemory(logicalDevice, &memoryAllocateInfo, nullptr, &memory));

    // If a pointer to the buffer data has been passed, map the buffer and copy over the data
    if (data) {
        map(size);
        copy(data);

        // If host coherency hasn't been requested, do a manual flush to make writes visible
        if ((properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
            flush(size);
        }

        unmap();
    }

    // Attach the memory to the buffer object
    bind();
}

Buffer::~Buffer() {
    unmap();
    vkDestroyBuffer(logicalDevice, buffer, nullptr);
    vkFreeMemory(logicalDevice, memory, nullptr);
}

void Buffer::bind(VkDeviceSize offset) {
    VK_CHECK(vkBindBufferMemory(logicalDevice, buffer, memory, offset));
}

void Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
    FE_ASSERT(buffer && memory && "Called map on buffer before create");

    VK_CHECK(vkMapMemory(logicalDevice, memory, offset, size, 0, &mapped));
}

void Buffer::unmap() {
    if (mapped) {
        vkUnmapMemory(logicalDevice, memory);
        mapped = nullptr;
    }
}

void Buffer::copy(const void* data, VkDeviceSize size, VkDeviceSize offset) {
    FE_ASSERT(mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
        std::memcpy(mapped, data, this->size);
    } else {
        std::memcpy(static_cast<std::byte*>(mapped) + offset, data, size);
    }
}

int Buffer::compare(const void* data, VkDeviceSize size, VkDeviceSize offset) {
    FE_ASSERT(mapped && "Cannot compare to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
        return std::memcmp(mapped, data, this->size);
    } else {
        return std::memcmp(static_cast<std::byte*>(mapped) + offset, data, size);
    }
}

void Buffer::extract(void* data, VkDeviceSize size, VkDeviceSize offset) {
    FE_ASSERT(mapped && "Cannot extract from unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
        std::memcpy(data, mapped, this->size);
    } else {
        std::memcpy(data, static_cast<std::byte*>(mapped) + offset, size);
    }
}

void Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedMemoryRange = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
    mappedMemoryRange.memory = memory;
    mappedMemoryRange.offset = offset;
    mappedMemoryRange.size = size;
    VK_CHECK(vkFlushMappedMemoryRanges(logicalDevice, 1, &mappedMemoryRange));
}

void Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedMemoryRange = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
    mappedMemoryRange.memory = memory;
    mappedMemoryRange.offset = offset;
    mappedMemoryRange.size = size;
    VK_CHECK(vkInvalidateMappedMemoryRanges(logicalDevice, 1, &mappedMemoryRange));
}

VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorBufferInfo{ buffer, offset, size, };
}

VkDeviceSize Buffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

uint32_t Buffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags requiredProperties) {
    const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();
    auto memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        uint32_t memoryTypeBits = 1 << i;

        if (typeFilter & memoryTypeBits && (memoryProperties.memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find a valid memory type for buffer");
}

void Buffer::InsertBufferMemoryBarrier(VkCommandBuffer commandBuffer, VkBuffer buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
                                       VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDeviceSize offset, VkDeviceSize size) {
    VkBufferMemoryBarrier bufferMemoryBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    bufferMemoryBarrier.srcAccessMask = srcAccessMask;
    bufferMemoryBarrier.dstAccessMask = dstAccessMask;
    bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    bufferMemoryBarrier.buffer = buffer;
    bufferMemoryBarrier.offset = offset;
    bufferMemoryBarrier.size = size;
    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
}

std::unique_ptr<Buffer> Buffer::StageToDeviceBuffer(VkBufferUsageFlags usage, VkDeviceSize size, const void* data) {
    Buffer stagingBuffer{size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, data};
    auto deviceBuffer = std::make_unique<Buffer>(size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    CommandBuffer commandBuffer{true};

    VkBufferCopy copyRegion = {};
    copyRegion.size = stagingBuffer.getSize();
    vkCmdCopyBuffer(commandBuffer, stagingBuffer, *deviceBuffer, 1, &copyRegion);

    commandBuffer.submitIdle();

    return deviceBuffer;
}

std::unique_ptr<Buffer> Buffer::DeviceToStageBuffer(const Buffer& deviceBuffer) {
    auto stagingBuffer = std::make_unique<Buffer>(deviceBuffer, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    CommandBuffer commandBuffer{true};

    VkBufferCopy copyRegion = {};
    copyRegion.size = stagingBuffer->getSize();
    vkCmdCopyBuffer(commandBuffer, *stagingBuffer, deviceBuffer, 1, &copyRegion);

    commandBuffer.submitIdle();

    return stagingBuffer;
}
