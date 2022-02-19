#pragma once

#include "Vulkan.hpp"
#include "AllocatedBuffer.hpp"
#include "Descriptors.hpp"
#include "Offscreen.hpp"

namespace Fusion {
    class SwapChain;

    struct FUSION_API GlobalUbo {
        alignas(16) glm::mat4 projection;
        alignas(16) glm::mat4 view;
        alignas(16) glm::vec3 lightDirection;
    };

    class FUSION_API Renderer {
    public:
        Renderer(Vulkan& vulkan);
        ~Renderer();

        glm::vec3& getColor() { return color; }
        glm::vec3& getLightDirection() { return lightDirection; }

        DescriptorLayoutCache& getDescriptorLayoutCache() { return descriptorLayoutCache; };
        DescriptorAllocator& getGlobalAllocator() { return globalAllocator; }
        DescriptorAllocator& getCurrentDynamicAllocator() { return dynamicAllocators[currentFrame]; }

        vk::CommandBuffer& getCurrentCommandBuffer() { return offscreenBuffer[currentFrame]; }
        AllocatedBuffer& getCurrentUniformBuffer() { return uniformBuffers[currentFrame]; }

        const vk::DescriptorSet getCurrentGlobalDescriptorSets() const { return globalDescriptorSets[currentFrame]; }
        const vk::DescriptorSetLayout& getGlobalDescriptorLayoutSet() const { return globalDescriptorSetLayout; }

        const std::unique_ptr<SwapChain>& getSwapChain() const { return swapChain; }
        Offscreen& getOffscreen() { return offscreen; }

        uint32_t getFrameIndex() const { return currentFrame; }
        bool isFrameInProgress() const { return isFrameStarted; }

        bool beginFrame();

        vk::CommandBuffer beginOffscreenRenderPass();
        void endOffscreenRenderPass(vk::CommandBuffer& commandBuffer);

        vk::CommandBuffer beginSwapChainRenderPass();
        void endSwapChainRenderPass(vk::CommandBuffer& commandBuffer);

        void endFrame();

    private:
        void createCommandBuffers();
        void createUniformBuffers();
        void createDescriptorSets();
        void recreateSwapChain();

        Vulkan& vulkan;

        std::unique_ptr<SwapChain> swapChain;
        std::vector<vk::CommandBuffer> commandBuffers;
        Offscreen offscreen;
        std::vector<vk::CommandBuffer> offscreenBuffer;

        std::vector<AllocatedBuffer> uniformBuffers;
        vk::DescriptorSetLayout globalDescriptorSetLayout;
        std::vector<vk::DescriptorSet> globalDescriptorSets;

        /*

        vk::DescriptorSet textureDescriptorSet;
        vk::DescriptorSetLayout textureDescriptorLayout;*/


        DescriptorAllocator globalAllocator;
        std::vector<DescriptorAllocator> dynamicAllocators;
        DescriptorLayoutCache descriptorLayoutCache;

        glm::vec3 color{0.7f, 0.85f, 1.0f};
        glm::vec3 lightDirection{1, -3, -1};

        uint32_t currentImage{0};
        uint32_t currentFrame{0};
        bool isFrameStarted{false};
    };
}

