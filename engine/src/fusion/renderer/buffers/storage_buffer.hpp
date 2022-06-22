#pragma once

#include "fusion/renderer/descriptors/descriptor.hpp"
#include "fusion/renderer/buffers/buffer.hpp"

namespace fe {
    class StorageBuffer : public Descriptor, public Buffer {
    public:
        explicit StorageBuffer(VkDeviceSize size, const void* data = nullptr);

        void update(const void* newData);

        WriteDescriptorSet getWriteDescriptor(uint32_t binding, VkDescriptorType descriptorType, const std::optional<OffsetSize>& offsetSize) const override;

        static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stage, uint32_t count);
    };
}
