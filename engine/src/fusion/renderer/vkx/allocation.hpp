#pragma once

#include <vulkan/vulkan.hpp>

namespace vkx {

// A wrapper class for an allocation, either an Image or Buffer.  Not intended to be used used directly
// but only as a base class providing common functionality for the classes below.
//
// Provides easy to use mechanisms for mapping, unmapping and copying host data to the device memory
struct Allocation {
    vk::Device device;
    vk::DeviceMemory memory;
    vk::DeviceSize size{ 0 };
    vk::DeviceSize alignment{ 0 };
    vk::DeviceSize allocSize{ 0 };
    void* mapped{ nullptr };

    /** @brief Memory propertys flags to be filled by external source at buffer creation (to query at some later point) */
    vk::MemoryPropertyFlags memoryPropertyFlags;

    /**
     * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
     *
     * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
     * buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the buffer mapping call
     */
    template <typename T = void>
    inline T* map(size_t offset = 0, VkDeviceSize size = VK_WHOLE_SIZE) {
        mapped = device.mapMemory(memory, offset, size, vk::MemoryMapFlags());
        return reinterpret_cast<T*>(mapped);
    }

    /**
     * Unmap a mapped memory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    inline void unmap() {
        device.unmapMemory(memory);
        mapped = nullptr;
    }

    /**
     * Copies the specified data to the mapped buffer. Default value writes whole buffer range
     *
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
     * @param data Pointer to the data to copy range.
     * @param offset (Optional) Byte offset from beginning of mapped region
     *
     */
    inline void copy(size_t size, const void* data, VkDeviceSize offset = 0) const { memcpy(static_cast<uint8_t*>(mapped) + offset, data, size); }

    template <typename T>
    inline void copy(const T& data, VkDeviceSize offset = 0) const {
        copy(sizeof(T), &data, offset);
    }

    template <typename T>
    inline void copy(const std::vector<T>& data, VkDeviceSize offset = 0) const {
        copy(sizeof(T) * data.size(), data.data(), offset);
    }

    /**
     * Flush a memory range of the buffer to make it visible to the device
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the flush call
     */
    void flush(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0) {
        return device.flushMappedMemoryRanges(vk::MappedMemoryRange{ memory, offset, size });
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate the complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the invalidate call
     */
    void invalidate(vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0) {
        return device.invalidateMappedMemoryRanges(vk::MappedMemoryRange{ memory, offset, size });
    }

    virtual void destroy() {
        if (mapped != nullptr) {
            unmap();
        }
        if (memory) {
            device.freeMemory(memory);
            memory = vk::DeviceMemory();
        }
    }
};
}  // namespace vkx
