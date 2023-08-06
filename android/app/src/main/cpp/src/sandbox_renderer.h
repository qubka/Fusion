#pragma once

#include "fusion/graphics/renderer.h"
#include "fusion/imgui/imgui_subrender.h"
#include "fusion/debug/grid_subrender.h"
#include "fusion/skybox/atmosphere_subrender.h"
#include "fusion/skybox/skybox_subrender.h"
#include "fusion/models/mesh_subrender.h"
#include "fusion/ligthing/light_subrender.h"

namespace fe {
    class FUSION_API SandboxRenderer : public Renderer {
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
