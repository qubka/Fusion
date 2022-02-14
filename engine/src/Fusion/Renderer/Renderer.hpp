#pragma once

#include "Vulkan.hpp"

namespace Fusion {
    class SwapChain;
    class AllocatedBuffer;
    class DescriptorPool;
    class DescriptorLayout;

    struct FUSION_API UniformBufferObject {
        alignas(16) glm::mat4 perspective;
        alignas(16) glm::mat4 orthogonal;
    };

    class FUSION_API Renderer {
    public:
        Renderer(Vulkan& vulkan);
        ~Renderer();

        const Vulkan& getVulkan() const { return vulkan; }
        glm::vec3& getColor() { return color; }

        const vk::DescriptorSetLayout& getGlobalLayoutSet() const;
        const std::unique_ptr<SwapChain>& getSwapChain() const;

        const vk::CommandBuffer& getCurrentCommandBuffer();
        const vk::DescriptorSet& getCurrentDescriptorSet();
        const std::unique_ptr<AllocatedBuffer>& getCurrentUniformBuffer();
        uint32_t getFrameIndex() const;
        bool isFrameInProgress() const;

        vk::CommandBuffer beginFrame();
        void beginSwapChainRenderPass(vk::CommandBuffer& commandBuffer);
        void endSwapChainRenderPass(vk::CommandBuffer& commandBuffer);
        void endFrame(vk::CommandBuffer& commandBuffer);

    private:
        void createCommandBuffers();
        void createUniformBuffers();
        void createDescriptorSets();
        void recreateSwapChain();

        Vulkan& vulkan;

        std::unique_ptr<SwapChain> swapChain;
        std::vector<vk::CommandBuffer> commandBuffers;
        std::vector<std::unique_ptr<AllocatedBuffer>> uniformBuffers;

        std::vector<vk::DescriptorSet> globalDescriptorSets;
        std::unique_ptr<DescriptorPool> globalPool;
        std::unique_ptr<DescriptorLayout> globalLayout;

        glm::vec3 color{0.7f, 0.85f, 1.0f};

        uint32_t currentImage{0};
        uint32_t currentFrame{0};
        bool isFrameStarted{false};
    };
}

