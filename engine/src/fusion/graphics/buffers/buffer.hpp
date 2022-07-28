#pragma once

namespace fe {
    class LogicalDevice;
    /**
    * @brief Encapsulates access to a Vulkan buffer backed up by device memory.
    * @note To be filled by an external source like the VulkanDevice.
    */
    class CommandBuffer;
    struct Buffer {
        enum class Status : uint8_t { Reset, Changed, Normal };

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
         * Attach the allocated memory block to the buffer.
         *
         * @param offset (Optional) Byte offset (from the beginning) for the memory region to bind.
         */
        void bind(VkDeviceSize offset = 0);

        /**
         * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
         * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
         * buffer range.
         * @param offset (Optional) Byte offset from beginning.
         */
        void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        /**
         * Unmap a mapped memory range.
         * @note Does not return a result as vkUnmapMemory can't fail.
         */
        void unmap();

        /**
         * Copies the specified data to the mapped buffer. Default value writes whole buffer range.
         *
         * @param data Pointer to the data to copy range.
         * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to copy the complete buffer.
         * @param offset (Optional) Byte offset from beginning of mapped region.
         *
         */
        void copy(const void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        /**
         * Compares the specified data in the mapped buffer. Default value compares whole buffer range.
         *
         * @param data Pointer to the data to copy range.
         * @param size (Optional) Size of the data to compare. Pass VK_WHOLE_SIZE to compare the complete buffer.
         * @param offset (Optional) Byte offset from beginning of mapped region.
         *
         * @return Returns an integral value indicating the relationship between the content of the memory blocks.
         */
        int compare(const void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        /**
         * Extracts the specified data from the mapped buffer. Default value extract whole buffer range.
         *
         * @param data Pointer to the data to copy range.
         * @param size (Optional) Size of the data to extract. Pass VK_WHOLE_SIZE to exract the complete buffer.
         * @param offset (Optional) Byte offset from beginning of mapped region.
         *
         * @return Returns an integral value indicating the relationship between the content of the memory blocks.
         */
        void extract(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        /**
         * Flush a memory range of the buffer to make it visible to the device.
         *
         * @note Only required for non-coherent memory.
         *
         * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the complete buffer range.
         * @param offset (Optional) Byte offset from beginning.
         */
        void flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        /**
         * Invalidate a memory range of the buffer to make it visible to the host.
         *
         * @note Only required for non-coherent memory.
         *
         * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate the complete buffer range.
         * @param offset (Optional) Byte offset from beginning.
         */
        void invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        /**
         * Create a buffer info descriptor.
         *
         * @param size (Optional) Size of the memory range of the descriptor.
         * @param offset (Optional) Byte offset from beginning.
         *
         * @return VkDescriptorBufferInfo of specified offset and range.
         */
        VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        const VkBuffer& getBuffer() const { return buffer; }
        VkDeviceSize getSize() const { return size; }
        const VkDeviceMemory& getBufferMemory() const { return memory; }
        void* getMappedMemory() const { return mapped; }
        VkBufferUsageFlags getUsageFlags() const { return usageFlags; }
        VkMemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags; }

        operator bool() const { return buffer != VK_NULL_HANDLE; }
        operator const VkBuffer&() const { return buffer; }

        static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags requiredProperties);
        static void InsertBufferMemoryBarrier(VkCommandBuffer commandBuffer, VkBuffer buffer, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        static std::unique_ptr<Buffer> StageToDeviceBuffer(VkBufferUsageFlags usage, VkDeviceSize size, const void* data);
        static std::unique_ptr<Buffer> DeviceToStageBuffer(const Buffer& deviceBuffer);

    private:
        /**
         * Returns the minimum instance size required to be compatible with devices minOffsetAlignment.
         *
         * @param instanceSize The size of an instance.
         * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
         * minUniformBufferOffsetAlignment).
         *
         * @return VkResult of the buffer mapping call.
         */
        static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

    protected:
        const LogicalDevice& logicalDevice;

        void* mapped{ nullptr };
        VkBuffer buffer;
        VkDeviceMemory memory{ VK_NULL_HANDLE };
        VkDeviceSize size;

        /** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
        VkBufferUsageFlags usageFlags;
        /** @brief Memory property flags to be filled by external source at buffer creation (to query at some later point) */
        VkMemoryPropertyFlags memoryPropertyFlags;
    };
}