#pragma once

#include "vkx/helpers.hpp"
#include "vkx/filesystem.hpp"
#include "vkx/model.hpp"
#include "vkx/shaders.hpp"
#include "vkx/pipelines.hpp"
#include "vkx/texture.hpp"
#include "vkx/swapchain.hpp"
#include "vkx/descriptors.hpp"
#include "vkx/offscreen.hpp"

namespace fe {
    struct GlobalUbo {
        alignas(16) glm::mat4 projection;
        alignas(16) glm::mat4 view;
        alignas(16) glm::vec3 lightDirection;
    };

    class Renderer {
    public:
        Renderer(const vkx::Context& context) : context{context} { }

        void create(const vk::Extent2D& size);
        void destroy();

        glm::vec3& getColor() { return color; }
        //glm::vec3& getLightDirection() { return lightDirection; }

        vkx::DescriptorLayoutCache& getDescriptorLayoutCache() { return descriptorLayoutCache; };
        vkx::DescriptorAllocator& getGlobalAllocator() { return globalAllocator; }
        vkx::DescriptorAllocator& getCurrentDynamicAllocator() { return dynamicAllocators[currentFrame]; }

        vk::CommandBuffer& getCurrentCommandBuffer() { return commandBuffers[currentFrame]; }
        vkx::Buffer& getCurrentUniformBuffer() { return uniformBuffers[currentFrame]; }

        const vk::DescriptorSet getCurrentGlobalDescriptorSets() const { return globalDescriptorSets[currentFrame]; }
        const vk::DescriptorSetLayout& getGlobalDescriptorLayoutSet() const { return globalDescriptorSetLayout; }

        const vkx::Offscreen& getOffscreen() const { return offscreen; }
        const vkx::SwapChain& getSwapChain() const { return swapChain; }

        uint32_t getFrameIndex() const { return currentFrame; }
        bool isFrameInProgress() const { return frameStarted; }

        uint32_t beginFrame();

        void beginRenderPass(uint32_t frameIndex);
        void endRenderPass(uint32_t frameIndex);

        void endFrame(uint32_t frameIndex);

    private:
        const vkx::Context& context;

        vkx::Offscreen offscreen{ context };
        vkx::SwapChain swapChain{ context };

        vk::CommandPool commandPool;
        std::vector<vk::CommandBuffer> commandBuffers;

        std::vector<vkx::Buffer> uniformBuffers;
        vk::DescriptorSetLayout globalDescriptorSetLayout;
        std::vector<vk::DescriptorSet> globalDescriptorSets;

        std::vector<vkx::DescriptorAllocator> dynamicAllocators;
        vkx::DescriptorAllocator globalAllocator{ context.device };
        vkx::DescriptorLayoutCache descriptorLayoutCache{ context.device };

        std::array<vk::ClearValue, 2> clearValues{};
        glm::vec3 color{0.7f, 0.85f, 1.0f};
        //glm::vec3 lightDirection{1, -3, -1};

        uint32_t currentImage{ 0 };
        uint32_t currentFrame{ 0 };
        bool frameStarted{ false };

        void createCommandBuffers();
        void createUniformBuffers();
        void createDescriptorSets();
        void recreateSwapChain();

        void setRenderPass(const vk::CommandBuffer& commandBuffer, const vk::RenderPass& renderPass, const vk::Framebuffer& framebuffer, const vk::Extent2D& extent);
    };
}
