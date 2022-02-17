#pragma once

#include "Vulkan.hpp"
#include "AllocatedBuffer.hpp"
#include "Descriptors.hpp"

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

        DescriptorLayoutCache& getDescriptorLayoutCache() { return descriptorLayoutCache; };
        DescriptorAllocator& getGlobalAllocator() { return globalAllocator; }
        DescriptorAllocator& getCurrentDynamicAllocator() { return dynamicAllocators[currentFrame]; }
        vk::CommandBuffer& getCurrentCommandBuffer() { return commandBuffers[currentFrame]; }
        AllocatedBuffer& getCurrentUniformBuffer() { return uniformBuffers[currentFrame]; }

        const vk::DescriptorSet getCurrentGlobalDescriptorSets() const { return globalDescriptorSets[currentFrame]; }
        const vk::DescriptorSetLayout& getGlobalDescriptorLayoutSet() const { return globalDescriptorSetLayout; }

        const std::unique_ptr<SwapChain>& getSwapChain() const { return swapChain; }

        uint32_t getFrameIndex() const { return currentFrame; }
        bool isFrameInProgress() const { return isFrameStarted; }

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
        vk::DescriptorSetLayout globalDescriptorSetLayout;
        std::vector<vk::DescriptorSet> globalDescriptorSets;

        /*

        vk::DescriptorSet textureDescriptorSet;
        vk::DescriptorSetLayout textureDescriptorLayout;*/


        DescriptorAllocator globalAllocator;
        std::vector<DescriptorAllocator> dynamicAllocators;
        DescriptorLayoutCache descriptorLayoutCache;

        glm::vec3 color{0.7f, 0.85f, 1.0f};

        uint32_t currentImage{0};
        uint32_t currentFrame{0};
        bool isFrameStarted{false};
    };
}

