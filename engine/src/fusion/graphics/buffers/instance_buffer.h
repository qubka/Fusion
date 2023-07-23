#pragma once

#include "fusion/graphics/buffers/buffer.h"

namespace fe {
    class InstanceBuffer : public Buffer {
    public:
        explicit InstanceBuffer(VkDeviceSize size);

        void update(const CommandBuffer& commandBuffer, const void* newData);
    };
}
