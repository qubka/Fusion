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
                    {1, "swapchain", Attachment::Type::Swapchain}
            };
            std::vector<SubpassType> renderpassSubpasses0 = {
                    {0, {0, 1}}
            };
            addRenderStage(std::make_unique<RenderStage>(renderpassAttachments0, renderpassSubpasses0));
        }
        ~MainRenderer() override = default;

        void start() override {
            addSubrender<GridRenderer>({0, 0});
            addSubrender<ImGuiSubrender>({0, 0});
        }

        void update(float dt) override {
        }

    private:
    };
}
