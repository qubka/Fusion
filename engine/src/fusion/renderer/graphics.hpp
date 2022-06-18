#pragma once

#include "renderer.hpp"
#include "window.hpp"

#include "fusion/renderer/vkx/helpers.hpp"
#include "fusion/renderer/vkx/filesystem.hpp"
#include "fusion/renderer/vkx/model.hpp"
#include "fusion/renderer/vkx/shaders.hpp"
#include "fusion/renderer/vkx/pipelines.hpp"
#include "fusion/renderer/vkx/texture.hpp"
#include "fusion/renderer/vkx/benchmark.hpp"

namespace fe {
    class Graphics {
    public:
        Graphics();
        ~Graphics();

        void update();

        /**
         * Takes a screenshot of the current image of the display and saves it into a image file.
         * @param filename The file to save the screenshot as.
         */
        void captureScreenshot(const std::string& filename) const;

    private:
        void createPipelineCache();
        void resetRenderStages();
        void recreateSwapchain();
        void recreateCommandBuffers();
        void recreatePass(RenderStage &renderStage);
        void recreateAttachmentsMap();
        bool startRenderpass(RenderStage &renderStage);
        void endRenderpass(RenderStage &renderStage);

        std::unique_ptr<Renderer> renderer;
        std::map<std::string, const Descriptor *> attachments;
        std::unique_ptr<Swapchain> swapchain;

        std::map<std::thread::id, std::shared_ptr<CommandPool>> commandPools;
        /// Timer used to remove unused command pools.
        ElapsedTime elapsedPurge;

        VkPipelineCache pipelineCache = VK_NULL_HANDLE;
        std::vector<VkSemaphore> presentCompletes;
        std::vector<VkSemaphore> renderCompletes;
        std::vector<VkFence> flightFences;
        std::size_t currentFrame = 0;
        bool framebufferResized = false;

        std::vector<vk::CommandBuffer> commandBuffers;

        std::unique_ptr<Instance> instance;
        std::unique_ptr<PhysicalDevice> physicalDevice;
        std::unique_ptr<Surface> surface;
        std::unique_ptr<LogicalDevice> logicalDevice;
    };
}
