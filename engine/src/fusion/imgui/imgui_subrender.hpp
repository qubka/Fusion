#pragma once

#include "imgui_object.hpp"

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/input/codes.hpp"

namespace fe {
    class Window;
    class ImGuiSubrender final : public Subrender {
    public:
        explicit ImGuiSubrender(const Pipeline::Stage& pipelineStage);
        ~ImGuiSubrender() override;

        void onUpdate() override;
        void onRender(const CommandBuffer& commandBuffer) override;

    private:
        void setupEvents(bool connect);
        void setupKeyCodes();
        void setupStyle();

        void onMouseButtonEvent(MouseButton button, InputAction action, bitmask::bitmask<InputMod> mods);
        void onMouseMotionEvent(const glm::vec2& pos);
        void onMouseScrollEvent(const glm::vec2& offset);
        void onMouseEnterEvent(bool entered);
        void onKeyEvent(Key key, InputAction action, Key scan, bitmask::bitmask<InputMod> mods);
        void onCharInputEvent(uint32_t chr);
        void onFocusEvent(bool focuses);
        void onSizeChange(const glm::uvec2& size);

        static const char* GetClipboardText(void* userData);
        static void SetClipboardText(void* userData, const char* text);
        static int KeyToImGuiKey(Key key);

    private:
        PipelineGraphics pipeline;

        DescriptorsHandler descriptorSet;
        PushHandler pushObject;

        ImGuiObject canvasObject;

        float fontSize;
        float fontScale;

        Window* window{ nullptr };
        Window* currentWindow{ nullptr };
        glm::vec2 lastValidMousePos{ -FLT_MAX };
    };
}
