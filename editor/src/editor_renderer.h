#pragma once

#include "fusion/graphics/renderer.h"
#include "fusion/imgui/imgui_subrender.h"
#include "fusion/debug/grid_subrender.h"
#include "fusion/skybox/atmosphere_subrender.h"
#include "fusion/skybox/skybox_subrender.h"
#include "fusion/models/mesh_subrender.h"
#include "fusion/models/mesh2_subrender.h"
#include "fusion/ligthing/light_subrender.h"
//#include "fusion/post/deferred/deferred_subrender.h"
#include "fusion/debug/debug_subrender.h"

namespace fe {
    class EditorRenderer : public Renderer {
    public:
        EditorRenderer() {
            std::vector<Attachment> renderpassAttachments0 = {
                    {0, "scene_depth", Attachment::Type::Depth},
                    /*{1, "position", Attachment::Type::Image, false, VK_FORMAT_R16G16B16A16_SFLOAT},
                    {2, "diffuse", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM},
                    {3, "normal", Attachment::Type::Image, false, VK_FORMAT_R16G16B16A16_SFLOAT},
                    {4, "material", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM},*/
                    {1, "scene_image", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, { 0.27f, 0.27f, 0.27f, 1.0f }},
            };
            std::vector<SubpassType> renderpassSubpasses0 = {
                   // {0, {0, 1, 2, 3, 4}},
                    {0, {0, 1}},
            };//

            addRenderStage(std::make_unique<RenderStage>(std::move(renderpassAttachments0), std::move(renderpassSubpasses0)));

            std::vector<Attachment> renderpassAttachments1 = {
                    {0, "game_depth", Attachment::Type::Depth},
                    {1, "game_image", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
            };
            std::vector<SubpassType> renderpassSubpasses1 = {
                    {0, {0, 1}},
            };

            addRenderStage(std::make_unique<RenderStage>(std::move(renderpassAttachments1), std::move(renderpassSubpasses1)));

            std::vector<Attachment> renderpassAttachments2 = {
                    {0, "swapchain", Attachment::Type::Swapchain},
            };
            std::vector<SubpassType> renderpassSubpasses2 = {
                    {0, {0}}
            };

            addRenderStage(std::make_unique<RenderStage>(std::move(renderpassAttachments2), std::move(renderpassSubpasses2)));
        }
        ~EditorRenderer() override = default;

    private:
        void onStart() override {
            addSubrender<Mesh2Subrender>({0, 0});
            addSubrender<LightSubrender>({0, 0});
            addSubrender<DebugSubrender>({0, 0});
            addSubrender<GridSubrender>({0, 0});

            addSubrender<Mesh2Subrender>({1, 0});
            addSubrender<LightSubrender>({1, 0});
            //addSubrender<SkyboxSubrender>({1, 0});
            //addSubrender<AtmosphereSubrender>({1, 0});

            addSubrender<ImGuiSubrender>({2, 0});
        }

        void onUpdate() override {

        }
    };
}
