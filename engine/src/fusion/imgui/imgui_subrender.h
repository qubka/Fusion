#pragma once

#include "fusion/imgui/imgui_object.h"

#include "fusion/graphics/graphics.h"
#include "fusion/graphics/subrender.h"
#include "fusion/graphics/buffers/push_handler.h"
#include "fusion/graphics/buffers/uniform_handler.h"
#include "fusion/graphics/pipelines/pipeline_graphics.h"
#include "fusion/graphics/descriptors/descriptors_handler.h"

namespace fe {
    class Window;
    class Texture2d;
    class ImGuiSubrender final : public Subrender {
        typedef void* ImTextureID;
    public:
        explicit ImGuiSubrender(Pipeline::Stage pipelineStage);
        ~ImGuiSubrender() override;

    private:
        void onUpdate() override;
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        void setupEvents(bool connect);
        void setupKeyCodes();
        void setupStyle();
        void addIconFont();
        void rebuildFont();

        void onMouseButton(MouseButton button, InputAction action, bitmask::bitmask<InputMod> mods);
        void onMouseMotion(const glm::vec2& pos);
        void onMouseScroll(const glm::vec2& offset);
        void onMouseEnter(bool entered);
        void onKeyPress(Key key, InputAction action, Key scan, bitmask::bitmask<InputMod> mods);
        void onCharInput(uint32_t chr);
        void onWindowFocus(bool focuses);
        void onWindowResize(const glm::uvec2& size);

        static const char* GetClipboardText(void* userData);
        static void SetClipboardText(void* userData, const char* text);
        static int KeyToImGuiKey(Key key);

    private:
        PipelineGraphics pipeline;

        DescriptorsHandler descriptorSet;
        PushHandler pushObject;
        UniformHandler uniformBuffer;

        ImGuiObject canvasObject;

        std::vector<std::unique_ptr<Texture2d>> fontTextures;

        std::array<fst::unordered_flatmap<ImTextureID, VkDescriptorSet>, MAX_FRAMES_IN_FLIGHT> descriptorSets;
        std::array<fst::unordered_flatmap<ImTextureID, bool>, MAX_FRAMES_IN_FLIGHT> descriptorSetHasUpdated;

        float fontSize;
        float fontScale;

        Window* window{ nullptr };
        Window* currentWindow{ nullptr };
        glm::vec2 lastValidMousePos{ -FLT_MAX };
    };
}
