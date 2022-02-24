#pragma once

#include "vkx/helpers.hpp"
#include "vkx/filesystem.hpp"
#include "vkx/model.hpp"
#include "vkx/shaders.hpp"
#include "vkx/pipelines.hpp"
#include "vkx/texture.hpp"
#include "fusion/renderer/vkx/swapchain.hpp"
#include "vkx/ui.hpp"
#include "vkx/descriptors.hpp"
#include "fusion/renderer/vkx/framebuffer.hpp"

namespace fe {
    struct {
        alignas(16) glm::mat4 projection;
        alignas(16) glm::mat4 view;
        alignas(16) glm::vec3 lightDirection;
    } globalUbo;

    class Renderer {
    public:
        Renderer(const vkx::Context& context) : context{context} { }

        void create();
        void destroy();

        //glm::vec3& getColor() { return color; }
        //glm::vec3& getLightDirection() { return lightDirection; }

        vkx::DescriptorLayoutCache& getDescriptorLayoutCache() { return descriptorLayoutCache; };
        vkx::DescriptorAllocator& getGlobalAllocator() { return globalAllocator; }
        vkx::DescriptorAllocator& getCurrentDynamicAllocator() { return dynamicAllocators[currentFrame]; }

        //vk::CommandBuffer& getCurrentCommandBuffer() { return offscreenBuffer[currentFrame]; }
        vkx::Buffer& getCurrentUniformBuffer() { return uniformBuffers[currentFrame]; }

        const vk::DescriptorSet getCurrentGlobalDescriptorSets() const { return globalDescriptorSets[currentFrame]; }
        const vk::DescriptorSetLayout& getGlobalDescriptorLayoutSet() const { return globalDescriptorSetLayout; }

        const vkx::SwapChain& getSwapChain() const { return *swapChain; }

        uint32_t getFrameIndex() const { return currentFrame; }
        bool isFrameInProgress() const { return isFrameStarted; }

        bool beginFrame();

        vk::CommandBuffer beginRender();
        void endRender(vk::CommandBuffer& commandBuffer);

        void endFrame();

    private:
        const vkx::Context& context;
        const vk::Device& device{ context.device };

        vk::CommandPool commandPool;
        vkx::SwapChain* swapChain{ nullptr };
        vk::RenderPass renderPass;

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
        bool isFrameStarted{ false };

        void createRenderPass();
        void createCommandBuffers();
        void createUniformBuffers();
        void createDescriptorSets();
        void recreateSwapChain();

        // Color buffer format
        vk::Format colorformat{ vk::Format::eB8G8R8A8Unorm };
        // Depth buffer format...  selected during Vulkan initialization
        vk::Format depthFormat{ vk::Format::eUndefined };
    };
}
