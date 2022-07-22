#pragma once

#include "fusion/graphics/buffers/buffer.hpp"

namespace fe {
    class Pipeline;
    class CommandBuffer;
    class ImGuiObject {
        typedef void* ImTextureID;
    public:
        ImGuiObject() = default;
        ~ImGuiObject() = default;
        NONCOPYABLE(ImGuiObject);

        void cmdRender(const CommandBuffer& commandBuffer, const Pipeline& pipeline, std::map<ImTextureID, VkDescriptorSet>& descriptorSets);

    private:
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        int32_t vertexCount{ 0 };
        int32_t indexCount{ 0 };

        std::queue<std::unique_ptr<Buffer>> removePool;
    };
}
