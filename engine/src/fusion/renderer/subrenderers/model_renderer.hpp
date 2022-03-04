#pragma once

#include "base_renderer_system.hpp"
#include "fusion/renderer/vkx/context.hpp"

namespace fe {
    class ModelRenderer : public BaseRendererSystem {
        struct PushConstantData {
            alignas(16) glm::mat4 model{1};
            alignas(16) glm::mat4 normal{1};
        };

    public:
        ModelRenderer(const vkx::Context& context)
            : context{context} {}
        ~ModelRenderer() = default;

        void create() {
            createDescriptorSets();
            createPipelineLayout();
            createPipeline();
        }
        void destroy();

        void beginScene() override;
        //void draw(const glm::mat4& transform, const std::shared_ptr<Model>& mesh);
        void endScene() override;

    private:
        const vkx::Context& context;

        void createDescriptorSets();
        void createPipelineLayout();
        void createPipeline();

        vk::Pipeline pipeline;
        vk::PipelineLayout pipelineLayout;
        const vk::CommandBuffer* commandBuffer{nullptr};
    };
}
