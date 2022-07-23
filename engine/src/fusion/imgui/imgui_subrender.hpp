#pragma once

#include "fusion/imgui/imgui_object.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"
#include "fusion/graphics/buffers/uniform_handler.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/input/codes.hpp"

namespace fe {
    class Window;
    class Texture2d;
    class ImGuiSubrender final : public Subrender {
        typedef void* ImTextureID;
    public:
        explicit ImGuiSubrender(const Pipeline::Stage& stage);
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

        std::array<std::unordered_map<ImTextureID, VkDescriptorSet>, MAX_FRAMES_IN_FLIGHT> descriptorSets;
        std::unordered_map<ImTextureID, const VkDescriptorImageInfo*> descriptorImageInfos;
        std::array<std::unordered_map<ImTextureID, bool>, MAX_FRAMES_IN_FLIGHT> descriptorSetHasUpdated;

        float fontSize;
        float fontScale;

        Window* window{ nullptr };
        Window* currentWindow{ nullptr };
        glm::vec2 lastValidMousePos{ -FLT_MAX };
    };
}
