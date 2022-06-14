#pragma once

#include "fusion/renderer/vkx/context.hpp"
#include "fusion/renderer/vkx/model.hpp"

namespace fe {
    class Renderer;
    class MeshRenderer {
        struct PushConstantData {
            alignas(16) glm::mat4 model{1};
            alignas(16) glm::mat4 normal{1};
        };

    public:
        MeshRenderer(const vkx::Context& context, Renderer& renderer)
            : context{context}, renderer{renderer} {
            assert(!instance && "Model Renderer already exists!");
            if (instance == nullptr) {
                create();
                instance = this;
            }
        }

        ~MeshRenderer() {
            if (instance != nullptr) {
                destroy();
            }
            instance = nullptr;
        };

        void begin();
        void draw(const vkx::Model& model, glm::mat4 transform);
        void end();

        static MeshRenderer& Instance() { assert(instance && "Model Renderer was not initialized!"); return *instance; }

        vkx::Model* loadModel(const std::string& filename);

    private:
        void create() {
            createDescriptorSets();
            createPipelineLayout();
            createPipeline();
        }
        void destroy() {
            context.device.destroyPipelineLayout(pipelineLayout);
            context.device.destroyPipeline(pipeline);

            for (auto [path, model] : models) {
                model.destroy();
            }
            models.clear();
        }

        const vkx::Context& context;
        Renderer& renderer;

        void createDescriptorSets();
        void createPipelineLayout();
        void createPipeline();

        vk::Pipeline pipeline;
        vk::PipelineLayout pipelineLayout;
        vk::CommandBuffer* commandBuffer{ nullptr };

        std::unordered_map<std::string, vkx::Model> models;

        static MeshRenderer* instance;
    };
}
