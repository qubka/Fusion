#pragma once

#include "fusion/graphics/renderer.hpp"
#include "fusion/imgui/imgui_subrender.hpp"
#include "fusion/grid/grid_renderer.hpp"

namespace fe {
    class MainRenderer : public Renderer {
    public:
        MainRenderer() {
            std::vector<Attachment> renderpassAttachments0 = {
                    {0, "depth", Attachment::Type::Depth, false},
                    {1, "offscreen", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM}
            };
            std::vector<SubpassType> renderpassSubpasses0 = {
                    {0, {0, 1}}
            };
            addRenderStage(std::make_unique<RenderStage>(renderpassAttachments0, renderpassSubpasses0));

            std::vector<Attachment> renderpassAttachments1 = {
                    {0, "swapchain", Attachment::Type::Swapchain}
            };
            std::vector<SubpassType> renderpassSubpasses1 = {
                    {0, {0}}
            };
            addRenderStage(std::make_unique<RenderStage>(renderpassAttachments1, renderpassSubpasses1));
        }
        ~MainRenderer() override = default;

        void onStart() override {
            addSubrender<GridRenderer>({0, 0});
            addSubrender<ImGuiSubrender>({1, 0});
        }

        void onUpdate() override {
        }

    private:
    };
}
