#pragma once

#include "fusion/core/engine.hpp"
#include "fusion/devices/window.hpp"
#include "fusion/utils/elapsed_time.hpp"

#include "renderer.hpp"
#include "commands/command_buffer.hpp"
#include "commands/command_pool.hpp"
#include "devices/instance.hpp"
#include "devices/logical_device.hpp"
#include "devices/physical_device.hpp"
#include "devices/surface.hpp"
#include "vku.hpp"

namespace fe {
    /**
     * @brief Module that manages the Vulkan instance, Surface, Window and the renderpass structure.
     */
    class Graphics {
    public:
        Graphics();
        ~Graphics();
        NONCOPYABLE(Graphics);

        void update();

        static Graphics* Get() { return Instance_; }

        static std::string StringifyResultVk(VkResult result);
        static void CheckVk(VkResult result);

        /**
         * Takes a screenshot of the current image of the display and saves it into a image file.
         * @param filename The file to save the screenshot as.
         */
        void captureScreenshot(const std::filesystem::path& filename, size_t id = 0) const;

        const std::shared_ptr<CommandPool>& getCommandPool(const std::thread::id& threadId = std::this_thread::get_id());

        /**
         * Gets the current renderer.
         * @return The renderer.
         */
        Renderer* getRenderer() const { return renderer.get(); }

        /**
         * Sets the current renderer to a new renderer.
         * @param renderer The new renderer.
         */
        void setRenderer(std::unique_ptr<Renderer>&& renderer) { this->renderer = std::move(renderer); }

        const RenderStage* getRenderStage(uint32_t index) const;

        const Descriptor* getAttachment(const std::string &name) const;

        const PhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
        const LogicalDevice& getLogicalDevice() const { return logicalDevice; }
        const VkPipelineCache& getPipelineCache() const { return pipelineCache; }

        const Surface* getSurface(size_t id) const { return surfaces[id].get(); }
        const Swapchain* getSwapchain(size_t id) const { return swapchains[id].get(); }
        void setFramebufferResized(size_t id) { perSurfaceBuffers[id]->framebufferResized = true; }

    private:
        static Graphics* Instance_;

        void createPipelineCache();
        void resetRenderStages();
        void recreateSwapchain();
        void recreateCommandBuffers(size_t id);
        void recreatePass(size_t id, RenderStage& renderStage);
        void recreateAttachmentsMap();
        bool startRenderpass(size_t id, RenderStage& renderStage);
        void endRenderpass(size_t id, RenderStage& renderStage);

        void onWindowCreate(Window* window, bool create);

        std::unique_ptr<Renderer> renderer;
        std::map<std::string, const Descriptor*> attachments;
        std::vector<std::unique_ptr<Swapchain>> swapchains;

        std::map<std::thread::id, std::shared_ptr<CommandPool>> commandPools;
        /// Timer used to remove unused command pools.
        ElapsedTime elapsedPurge;

        struct PerSurfaceBuffers {
            std::vector<VkSemaphore> presentCompletes;
            std::vector<VkSemaphore> renderCompletes;
            std::vector<VkFence> flightFences;
            size_t currentFrame{ 0 };
            bool framebufferResized{ false };

            std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;
        };
        std::vector<std::unique_ptr<PerSurfaceBuffers>> perSurfaceBuffers;

        Instance instance;
        PhysicalDevice physicalDevice;
        LogicalDevice logicalDevice;
        VkPipelineCache pipelineCache{ VK_NULL_HANDLE };
        std::vector<std::unique_ptr<Surface>> surfaces;
    };
}
