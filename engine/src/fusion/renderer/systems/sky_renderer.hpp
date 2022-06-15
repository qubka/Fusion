#pragma once

#include "fusion/renderer/vkx/context.hpp"

namespace fe {
    class Renderer;
    class SkyRenderer {
        struct PushConstantData {
            glm::vec4 resolution_mouse{0};
            float time{0};
        };
    public:
        SkyRenderer(const vkx::Context& context, Renderer& renderer) : context{context}, renderer{renderer} {
            assert(!instance && "Sky Renderer already exists!");
            if (instance == nullptr) {
                create();
                instance = this;
            }
        }

        ~SkyRenderer() {
            if (instance != nullptr) {
                destroy();
            }
            instance = nullptr;
        };

        void begin();
        void draw();
        void end();

        static SkyRenderer& Instance() { assert(instance && "Sky Renderer was not initialized!"); return *instance; }

    private:
        void create() {
            createDescriptorSets();
            createPipelineLayout();
            createPipeline();
        }

        void destroy() {
            context.device.destroyPipelineLayout(pipelineLayout);
            context.device.destroyPipeline(pipeline);
        }

        const vkx::Context& context;
        Renderer& renderer;

        void createDescriptorSets();
        void createPipelineLayout();
        void createPipeline();

        vk::Pipeline pipeline;
        vk::PipelineLayout pipelineLayout;
        vk::CommandBuffer* commandBuffer{ nullptr };

        static SkyRenderer* instance;
    };
}
