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

                    {2, "scene", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM, {0.5f, 0.5f, 0.5f, 1.0f}},
                    //{3, "game", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM, {0.5f, 0.5f, 0.5f, 1.0f}}
            };
            std::vector<SubpassType> renderpassSubpasses0 = {
                    {0, {0, 2}},
                    {1, {0, 1}},
            };

            addRenderStage(std::make_unique<RenderStage>(renderpassAttachments0, renderpassSubpasses0));
        }
        ~EditorRenderer() override = default;

    private:
        void onStart() override {
            addSubrender<GridRenderer>({0, 0});
            addSubrender<ImGuiSubrender>({0, 1});
        }

        void onUpdate() override {
        }
    };
}
