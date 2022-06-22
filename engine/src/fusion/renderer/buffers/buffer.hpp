#pragma once

#include <volk.h>

namespace fe {
    class CommandBuffer;
    /**
     * @brief Interface that represents a buffer.
     */
    class Buffer {
    public:
        enum class Status { Reset, Changed, Normal };

        /**
         * Creates a new buffer with optional data.
         * @param size Size of the buffer in bytes.
         * @param usage Usage flag bitmask for the buffer (i.e. index, vertex, uniform buffer).
         * @param properties Memory properties for this buffer (i.e. device local, host visible, coherent).
         * @param data Pointer to the data that should be copied to the buffer after creation (optional, if not set, no data is copied over).
         */
        Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, const void* data = nullptr);
        virtual ~Buffer();

        /**
         * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
         * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
         * buffer range.
         * @param offset (Optional) Byte offset from beginning
         * @return VkResult of the buffer mapping call
         */
        void mapMemory(void** data, VkDeviceSize offset = 0) const;

        /**
         * Unmap a mapped memory range
         * @note Does not return a result as vkUnmapMemory can't fail
         */
        void unmapMemory() const;

        VkDeviceSize getSize() const { return size; }
        const VkBuffer& getBuffer() const { return buffer; }
        const VkDeviceMemory& getBufferMemory() const { return bufferMemory; }

        operator const VkBuffer&() const { return buffer; }

        static uint32_t FindMemoryType(uint32_t typeFilter, const VkMemoryPropertyFlags& requiredProperties);
        static void InsertBufferMemoryBarrier(const CommandBuffer& commandBuffer, const VkBuffer& buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

    protected:
        VkBuffer buffer{ VK_NULL_HANDLE };
        VkDeviceMemory bufferMemory{ VK_NULL_HANDLE };
        VkDeviceSize size{ 0 };
    };
}
