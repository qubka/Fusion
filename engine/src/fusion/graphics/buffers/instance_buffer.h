#pragma once

#include "fusion/graphics/buffers/buffer.h"

namespace fe {
    class FUSION_API InstanceBuffer : public Buffer {
    public:
        explicit InstanceBuffer(VkDeviceSize size);

        void update(const CommandBuffer& commandBuffer, const void* newData);
    };
}
