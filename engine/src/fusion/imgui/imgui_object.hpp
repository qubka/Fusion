#pragma once

#include "fusion/graphics/buffers/buffer.hpp"

namespace fe {
    class Pipeline;
    class CommandBuffer;
    class DescriptorsHandler;
    class ImGuiObject {
    public:
        ImGuiObject() = default;
        ~ImGuiObject() = default;
        NONCOPYABLE(ImGuiObject);

        void cmdRender(const CommandBuffer& commandBuffer, const Pipeline& pipeline, DescriptorsHandler& descriptorSet);

    private:
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        int32_t vertexCount{ 0 };
        int32_t indexCount{ 0 };

        std::queue<std::unique_ptr<Buffer>> removePool;
    };
}
