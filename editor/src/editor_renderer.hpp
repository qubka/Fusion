#pragma once

#include "fusion/graphics/renderer.hpp"
#include "fusion/imgui/imgui_subrender.hpp"
#include "fusion/debug/grid_subrender.hpp"
#include "fusion/skybox/atmosphere_subrender.hpp"
#include "fusion/skybox/skybox_subrender.hpp"
#include "fusion/models/mesh_subrender.hpp"
#include "fusion/ligthing/light_subrender.hpp"
#include "fusion/post/deferred/deferred_subrender.hpp"
#include "fusion/debug/debug_subrender.hpp"

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
                    {1, "scene_image", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM, { 0.27f, 0.27f, 0.27f, 1.0f }},
            };
            std::vector<SubpassType> renderpassSubpasses0 = {
                   // {0, {0, 1, 2, 3, 4}},
                    {0, {0, 1}},
            };

            addRenderStage(std::make_unique<RenderStage>(std::move(renderpassAttachments0), std::move(renderpassSubpasses0)));

            std::vector<Attachment> renderpassAttachments1 = {
                    {0, "game_depth", Attachment::Type::Depth},
                    {1, "game_image", Attachment::Type::Image, false, VK_FORMAT_R8G8B8A8_UNORM}
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
            //addSubrender<AtmosphereSubrender>({0, 0});
            addSubrender<MeshSubrender>({0, 0});
            addSubrender<LightSubrender>({0, 0});
            addSubrender<GridSubrender>({0, 0});
            addSubrender<DebugSubrender>({0, 0});
            //addSubrender<DeferredSubrender>({0, 1});
            //addSubrender<DeferredSubrender>({0, 1});
            //addSubrender<SkyboxSubrender>({0, 2});

            //addSubrender<SkyboxSubrender>({1, 0});

            addSubrender<ImGuiSubrender>({2, 0});
        }

        void onUpdate() override {
        }
    };
}
