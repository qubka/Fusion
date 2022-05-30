#pragma once

#include "fusion/renderer/vkx/context.hpp"

namespace fe {
    class Renderer;
    class GridRenderer {
    public:
        GridRenderer(const vkx::Context& context, Renderer& renderer)
            : context{context}, renderer{renderer} {
            assert(!instance && "Line Renderer already exists!");
            if (instance == nullptr) {
                create();
            }
            instance = this;
        }

        ~GridRenderer() {
            if (instance != nullptr) {
                destroy();
            }
            instance = nullptr;
        };

        void begin();
        void draw();
        void end();

        static GridRenderer& Instance() { assert(instance && "Line Renderer was not initialized!"); return *instance; }

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

        static GridRenderer* instance;
    };
}
