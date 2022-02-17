#pragma once

#include "Vulkan.hpp"
#include "AllocatedBuffer.hpp"

namespace Fusion {
    class SwapChain;
    class DescriptorPool;
    class DescriptorLayout;

    struct FUSION_API UniformBufferObject {
        alignas(16) glm::mat4 projection;
        alignas(16) glm::mat4 view;
        alignas(16) glm::vec3 lightDirection;
    };

    class FUSION_API Renderer {
    public:
        Renderer(Vulkan& vulkan);
        ~Renderer();

        glm::vec3& getColor() { return color; }

        const vk::DescriptorSetLayout& getGlobalLayoutSet() const;
        const std::unique_ptr<SwapChain>& getSwapChain() const { return swapChain; };

        const vk::CommandBuffer& getCommandBuffers(size_t index) const { return commandBuffers[index]; };
        const vk::DescriptorSet& getGlobalDescriptors(size_t index) const { return globalDescriptorSets[index]; };
        AllocatedBuffer& getUniformBuffers(size_t index) { return uniformBuffers[index]; };

        uint32_t getFrameIndex() const { FE_ASSERT(isFrameStarted && "cannot get frame index when frame not in progress"); return currentFrame; };
        bool isFrameInProgress() const { FE_ASSERT(isFrameStarted && "cannot get frame index when frame not in progress"); return isFrameStarted; };

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
        std::vector<AllocatedBuffer> uniformBuffers;

        std::vector<vk::DescriptorSet> globalDescriptorSets;
        std::unique_ptr<DescriptorPool> globalPool;
        std::unique_ptr<DescriptorLayout> globalLayout;

        glm::vec3 color{0.7f, 0.85f, 1.0f};

        uint32_t currentImage{0};
        uint32_t currentFrame{0};
        bool isFrameStarted{false};
    };
}

