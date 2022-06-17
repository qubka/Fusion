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
#include "vkx/ui.hpp"

#include "systems/mesh_renderer.hpp"
#include "systems/grid_renderer.hpp"
#include "systems/sky_renderer.hpp"

namespace fe {
    struct GlobalUbo {
        alignas(16) glm::mat4 projectionMatrix;
        alignas(16) glm::mat4 viewMatrix;
        alignas(16) glm::mat4 cameraMatrix;
        alignas(4) float frameTime;
    };

    class Renderer {
    public:
        Renderer(const vkx::Context& context) : context{context} { }

        void create(const vk::Extent2D& size, bool overlay = false);
        void destroy();

        glm::vec3& getColor() { return color; }
        glm::vec3& getLightDirection() { return lightDirection; }

        vkx::DescriptorLayoutCache& getDescriptorLayoutCache() { return descriptorLayoutCache; };
        vkx::DescriptorAllocator& getGlobalAllocator() { return globalAllocator; }
        vkx::DescriptorAllocator& getCurrentDynamicAllocator() { return dynamicAllocators[currentFrame]; }

        vk::RenderPass& getDrawRenderPass() { return offscreen.active ? offscreen.renderPass : swapChain.renderPass; }
        vk::CommandBuffer& getMainCommandBuffer() { return commandBuffers[currentFrame]; }
        vk::CommandBuffer& getCurrentCommandBuffer() { return offscreen.active ? offscreen.commandBuffers[currentFrame] : commandBuffers[currentFrame]; }
        vkx::Buffer& getCurrentUniformBuffer() { return uniformBuffers[currentFrame]; }
        vk::DescriptorSet& getCurrentFrameImage() { return offscreen.descriptorSets[currentFrame]; }

        const vk::DescriptorSet& getCurrentGlobalDescriptorSets() const { return globalDescriptorSets[currentFrame]; }
        const vk::DescriptorSetLayout& getGlobalDescriptorLayoutSet() const { return globalDescriptorSetLayout; }

        vkx::Offscreen& getOffscreen() { return offscreen; }
        vkx::SwapChain& getSwapChain() { return swapChain; }
        vkx::UIOverlay& getGUI() { return gui; }

        uint32_t getFrameIndex() const { return currentFrame; }
        bool isFrameInProgress() const { return frameStarted; }

        uint32_t beginFrame();

        void beginRenderPass(uint32_t frameIndex);
        void endRenderPass(uint32_t frameIndex);

        void endFrame(uint32_t frameIndex);

        bool beginGui(float dt) const;
        void endGui();

    private:
        const vkx::Context& context;

        vkx::Offscreen offscreen{ context };
        vkx::SwapChain swapChain{ context };
        vkx::UIOverlay gui{ context };

        vk::CommandPool commandPool;
        std::vector<vk::CommandBuffer> commandBuffers;

        std::vector<vkx::Buffer> uniformBuffers;
        vk::DescriptorSetLayout globalDescriptorSetLayout;
        std::vector<vk::DescriptorSet> globalDescriptorSets;

        std::vector<vkx::DescriptorAllocator> dynamicAllocators;
        vkx::DescriptorAllocator globalAllocator{ context.device };
        vkx::DescriptorLayoutCache descriptorLayoutCache{ context.device };

        std::array<vk::ClearValue, 2> clearValues{};
        glm::vec3 color{ 1.0f, 1.0f, 1.0f };
        glm::vec3 lightDirection{ 1, -3, -1 };

        MeshRenderer* meshRenderer;
        GridRenderer* gridRenderer;
        SkyRenderer* skyRenderer;

        uint32_t currentImage{ 0 };
        uint32_t currentFrame{ 0 };
        bool frameStarted{ false };

        void createCommandBuffers();
        void createUniformBuffers();
        void createDescriptorSets();
        void createGui();
        void recreateSwapChain();
    };
}
