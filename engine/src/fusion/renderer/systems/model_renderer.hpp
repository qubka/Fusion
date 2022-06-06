#pragma once

#include "fusion/renderer/vkx/context.hpp"
#include "fusion/renderer/vkx/model.hpp"

namespace fe {
    class Renderer;
    class ModelRenderer {
        struct PushConstantData {
            alignas(16) glm::mat4 model{1};
            alignas(16) glm::mat4 normal{1};
        };

    public:
        ModelRenderer(const vkx::Context& context, Renderer& renderer)
            : context{context}, renderer{renderer} {
            assert(!instance && "Model Renderer already exists!");
            if (instance == nullptr) {
                create();
            }
            instance = this;
        }

        ~ModelRenderer() {
            if (instance != nullptr) {
                destroy();
            }
            instance = nullptr;
        };

        void begin();
        void draw(const std::shared_ptr<vkx::model::Model>& model, const glm::mat4& transform);
        void end();

        static ModelRenderer& Instance() { assert(instance && "Model Renderer was not initialized!"); return *instance; }

        std::shared_ptr<vkx::model::Model> loadModel(const std::string& filename);

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
                model->destroy();
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

        std::unordered_map<std::string, std::shared_ptr<vkx::model::Model>> models;

        static ModelRenderer* instance;
    };
}
