#pragma once

#include "fusion/devices/window.hpp"
#include "fusion/utils/elapsed_time.hpp"

#include "fusion/graphics/devices/instance.hpp"
#include "fusion/graphics/devices/logical_device.hpp"
#include "fusion/graphics/devices/physical_device.hpp"
#include "fusion/graphics/devices/surface.hpp"
#include "fusion/graphics/pipelines/pipeline_cache.hpp"

#include "fusion/graphics/renderer.hpp"
#include "fusion/graphics/vku.hpp"
#include "fusion/core/module.hpp"

//#define MAX_FRAMES_IN_FLIGHT 2

namespace fe {
    class Renderer;
    class RenderStage;
    class CommandBuffer;
    class CommandPool;
    class Semaphore;
    class Fence;
    class Descriptor;
    class Swapchain;

    /**
     * @brief Module that manages the Vulkan's graphics context.
     */
    class Graphics : public Module::Registrar<Graphics> {
    public:
        Graphics();
        ~Graphics() override;

        void update(const Time& dt) override;

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
        const Descriptor* getAttachment(const std::string& name) const;

        const PhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
        const LogicalDevice& getLogicalDevice() const { return logicalDevice; }
        const VkPipelineCache& getPipelineCache() const { return pipelineCache; }

        const Surface* getSurface(size_t id) const { return surfaces[id].get(); }
        const Swapchain* getSwapchain(size_t id) const { return swapchains[id].get(); }

    private:
        void resetRenderStages();
        void recreateSwapchain();
        void recreatePass(size_t id, RenderStage& renderStage);
        void recreateAttachmentsMap();
        bool startRenderpass(size_t id, RenderStage& renderStage);
        bool endRenderpass(size_t id, RenderStage& renderStage);

        void onWindowCreate(Window* window, bool create);

        Instance instance;
        PhysicalDevice physicalDevice;
        LogicalDevice logicalDevice;
        PipelineCache pipelineCache;
        std::vector<std::unique_ptr<Surface>> surfaces;

        std::unique_ptr<Renderer> renderer;
        std::map<std::string, const Descriptor*> attachments;
        std::vector<std::unique_ptr<Swapchain>> swapchains;

        std::map<std::thread::id, std::shared_ptr<CommandPool>> commandPools;
        /// Timer used to remove unused command pools
        ElapsedTime elapsedPurge;

        struct PerSurfaceBuffers {
            explicit PerSurfaceBuffers(size_t currentFrame, size_t imageCount);
            size_t currentFrame;
            std::vector<CommandBuffer> commandBuffers;
            std::vector<Semaphore> presentCompletes;
            std::vector<Semaphore> renderCompletes;
            std::vector<Fence> flightFences;
            bool framebufferResized{ false };
        };
        std::vector<std::unique_ptr<PerSurfaceBuffers>> perSurfaceBuffers;
    };
}
