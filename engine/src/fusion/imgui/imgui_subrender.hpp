#pragma once

#include "fusion/graphics/subrender.hpp"

#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"

#include "fusion/input/mouse_buttons.hpp"
#include "fusion/input/key_codes.hpp"

namespace fe {
    class Window;
    class Buffer;
    class Image2d;

    class ImGuiSubrender final : public Subrender {
    public:
        ImGuiSubrender(const Pipeline::Stage& pipelineStage);
        ~ImGuiSubrender() override;

        void render(const CommandBuffer& commandBuffer) override;

    private:
        void drawFrame(const CommandBuffer& commandBuffer);
        void updateBuffers();
        void setupEvents(bool connect);

        void onMouseButtonEvent(MouseData data);
        void onMouseMotionEvent(const glm::vec2& pos);
        void onMouseScrollEvent(const glm::vec2& offset);
        void onMouseEnterEvent(bool entered);
        void onKeyEvent(KeyData data);
        void onCharInputEvent(uint32_t chr);
        void onFocusEvent(bool focuses);

    private:
        PipelineGraphics pipeline;

        DescriptorsHandler descriptorSet;
        PushHandler pushObject;

        std::unique_ptr<Image2d> font;
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        int32_t vertexCount{ 0 };
        int32_t indexCount{ 0 };

        Window* window{ nullptr };
        Window* currentWindow{ nullptr };
        glm::vec2 lastValidMousePos{ -FLT_MAX };
    };
}
