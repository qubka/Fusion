﻿#pragma once

#include "fusion/graphics/descriptors/descriptor.h"
#include "fusion/graphics/buffers/buffer.h"

namespace fe {
    class FUSION_API UniformBuffer final : public Descriptor, public Buffer {
    public:
        explicit UniformBuffer(VkDeviceSize size, const void* data = nullptr);

        void update(const void* newData);

        WriteDescriptorSet getWriteDescriptor(uint32_t binding, VkDescriptorType descriptorType, const std::optional<OffsetSize>& offsetSize) const override;

        static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stage, uint32_t count);
    };
}
