#pragma once

#include "fusion/graphics/buffers/buffer.h"

namespace fe {
    class Pipeline;
    class CommandBuffer;
    class FUSION_API ImGuiObject {
        typedef void* ImTextureID;
    public:
        ImGuiObject() = default;
        ~ImGuiObject() = default;
        NONCOPYABLE(ImGuiObject);

        void cmdRender(const CommandBuffer& commandBuffer, const Pipeline& pipeline, fst::unordered_flatmap<ImTextureID, VkDescriptorSet>& descriptorSets);

    private:
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        int32_t vertexCount{ 0 };
        int32_t indexCount{ 0 };

        std::queue<std::unique_ptr<Buffer>> removeQueue;
    };
}
