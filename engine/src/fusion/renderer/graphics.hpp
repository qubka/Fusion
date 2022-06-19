#pragma once

#include "renderstage.hpp"
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
        FE_NONCOPYABLE(Graphics);

        void update();

        /**
         * Takes a screenshot of the current image of the display and saves it into a image file.
         * @param filename The file to save the screenshot as.
         */
        void captureScreenshot(const std::string& filename) const;

        const std::shared_ptr<vk::CommandPool>& getCommandPool(const std::thread::id& threadId = std::this_thread::get_id());

        /**
         * Gets the current renderer.
         * @return The renderer.
         */
        Renderer* getRenderer() const { return renderer.get(); }

        /**
         * Sets the current renderer to a new renderer.
         * @param renderer The new renderer.
         */
        void setRenderer(std::unique_ptr<Renderer>&& ptr) { renderer = std::move(ptr); }

    private:
        void resetRenderStages();
        void recreateSwapchain();
        void recreateCommandBuffers();
        void recreateAttachmentsMap();
        void recreatePass(RenderStage& renderStage);

        bool startRenderpass(RenderStage& renderStage);
        void endRenderpass(RenderStage& renderStage);

        vkx::Context context;

        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<Window> window;

        //std::map<std::string, const Descriptor*> attachments;
        vkx::Swapchain swapchain{ context };

        /// Timer used to remove unused command pools.
        //ElapsedTime elapsedPurge;

        std::vector<vk::Semaphore> presentCompletes;
        std::vector<vk::Semaphore> renderCompletes;
        std::vector<vk::Fence> flightFences;
        size_t currentFrame{ 0 };
        bool framebufferResized{ false };

        std::vector<vk::CommandBuffer> commandBuffers;
    };
}
