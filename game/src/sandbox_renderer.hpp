#pragma once

#include "fusion/graphics/renderer.hpp"
#include "fusion/imgui/imgui_subrender.hpp"
#include "fusion/debug/grid_subrender.hpp"
#include "fusion/skybox/atmosphere_subrender.hpp"
#include "fusion/skybox/skybox_subrender.hpp"
#include "fusion/models/mesh_subrender.hpp"
#include "fusion/ligthing/light_subrender.hpp"

namespace fe {
    class SandboxRenderer : public Renderer {
    public:
        SandboxRenderer() {
            std::vector<Attachment> renderpassAttachments = {
                    {0, "depth", Attachment::Type::Depth},
                    {1, "swapchain", Attachment::Type::Swapchain},
            };
            std::vector<SubpassType> renderpassSubpasses = {
                    {0, {0, 1}}
            };

            addRenderStage(std::make_unique<RenderStage>(std::move(renderpassAttachments), std::move(renderpassSubpasses)));
        }
        ~SandboxRenderer() override = default;

    private:
        void onStart() override {
            addSubrender<MeshSubrender>({0, 0});
            addSubrender<LightSubrender>({0, 0});
            //addSubrender<SkyboxSubrender>({0, 0});
        }

        void onUpdate() override {

        }
    };
}
