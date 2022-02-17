#pragma once

#include "RendererSystemBase.hpp"
#include "Fusion/Renderer/Renderer.hpp"

namespace Fusion {
    class Mesh;
    class Texture;
    class Pipeline;

    struct PushConstantData {
        alignas(16) glm::mat4 model{1};
        alignas(16) glm::mat4 normal{1};
    };

    class MeshRenderer : public RendererSystemBase {
    public:
        MeshRenderer(Vulkan& vulkan, Renderer& renderer);
        ~MeshRenderer() override;

        void beginScene() override;
        void drawMesh(const glm::mat4& transform, const std::shared_ptr<Mesh>& mesh);
        void endScene() override;

    private:
        void createDescriptorSets();
        void createPipelineLayout();
        void createPipeline();

        Vulkan& vulkan;
        Renderer& renderer;

        //std::unique_ptr<Texture> texture;
        std::unique_ptr<Pipeline> pipeline;
        vk::PipelineLayout pipelineLayout;
        const vk::CommandBuffer* commandBuffer{nullptr};
    };
}
