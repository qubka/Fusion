#pragma once

#include "fusion/graphics/renderer.hpp"
#include "fusion/imgui/imgui_subrender.hpp"
#include "fusion/grid/grid_subrender.hpp"
#include "fusion/skybox/atmosphere_subrender.hpp"
#include "fusion/skybox/skybox_subrender.hpp"
//#include "fusion/models/model_subrender.hpp"

namespace fe {
    class EditorRenderer : public Renderer {
    public:
        EditorRenderer() {
            std::vector<Attachment> renderpassAttachments0 = {
                    {0, "scene_depth", Attachment::Type::Depth},
                    {1, "scene_image", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM, {0.27f, 0.27f, 0.27f, 1.0f}}
            };
            std::vector<SubpassType> renderpassSubpasses0 = {
                    {0, {0, 1}},
                    {1, {0, 1}},
            };

            addRenderStage(std::make_unique<RenderStage>(renderpassAttachments0, renderpassSubpasses0));

            std::vector<Attachment> renderpassAttachments1 = {
                    {0, "game_depth", Attachment::Type::Depth},
                    {1, "game_image", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM}
            };
            std::vector<SubpassType> renderpassSubpasses1 = {
                    {0, {0, 1}},
            };

            addRenderStage(std::make_unique<RenderStage>(renderpassAttachments1, renderpassSubpasses1));

            std::vector<Attachment> renderpassAttachments2 = {
                    {0, "swapchain", Attachment::Type::Swapchain},
            };
            std::vector<SubpassType> renderpassSubpasses2 = {
                    {0, {0}}
            };

            addRenderStage(std::make_unique<RenderStage>(renderpassAttachments2, renderpassSubpasses2));
        }
        ~EditorRenderer() override = default;

    private:
        void onStart() override {
            //addSubrender<AtmosphereSubrender>({0, 0});
            //addSubrender<ModelSubrender>({0, 0});
            addSubrender<SkyboxSubrender>({0, 0});
            addSubrender<GridSubrender>({0, 1});

            addSubrender<SkyboxSubrender>({1, 0});

            addSubrender<ImGuiSubrender>({2, 0});
        }

        void onUpdate() override {
        }
    };
}
