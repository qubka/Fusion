#pragma once

#include "RendererSystemBase.hpp"
#include "Fusion/Renderer/Renderer.hpp"

namespace Fusion {
    class Mesh;
    class Texture;
    class Pipeline;
    class DescriptorPool;
    class DescriptorLayout;

    struct PushConstantData {
        glm::mat4 model{1};
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

        std::vector<vk::DescriptorSet> textureDescriptorSets;
        std::unique_ptr<DescriptorPool> texturePool;
        std::unique_ptr<DescriptorLayout> textureLayout;
        std::unique_ptr<Texture> texture;
        std::unique_ptr<Pipeline> pipeline;
        vk::PipelineLayout pipelineLayout;
        const vk::CommandBuffer* commandBuffer{nullptr};
    };
}
