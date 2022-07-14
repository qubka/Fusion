#pragma once

#include "fusion/graphics/renderer.hpp"
#include "fusion/imgui/imgui_subrender.hpp"
#include "fusion/grid/grid_renderer.hpp"

namespace fe {
    class EditorRenderer : public Renderer {
    public:
        EditorRenderer() {
            std::vector<Attachment> renderpassAttachments0 = {
                    {0, "depth", Attachment::Type::Depth, false},
                    {1, "swapchain", Attachment::Type::Swapchain},
                    {2, "offscreen", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM, {0.5f, 0.5f, 0.5f, 1.0f}}
            };
            std::vector<SubpassType> renderpassSubpasses0 = {
                    {0, {0, 1}},
                    {1, {0, 2}},
            };
            addRenderStage(std::make_unique<RenderStage>(renderpassAttachments0, renderpassSubpasses0));
        }
        ~EditorRenderer() override = default;

        void onStart() override {
            addSubrender<ImGuiSubrender>({0, 0});
            addSubrender<GridRenderer>({0, 1});
        }

        void onUpdate() override {
        }

    private:
    };
}
