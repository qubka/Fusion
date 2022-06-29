#pragma once

#include "fusion/devices/window.hpp"

#include "fusion/graphics/devices/instance.hpp"
#include "fusion/graphics/devices/logical_device.hpp"
#include "fusion/graphics/devices/physical_device.hpp"
#include "fusion/graphics/devices/surface.hpp"
#include "fusion/graphics/pipelines/pipeline_cache.hpp"
#include "fusion/graphics/renderpass/sync_object.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/graphics/renderer.hpp"
#include "fusion/graphics/vku.hpp"
#include "fusion/core/module.hpp"

#define MAX_FRAMES_IN_FLIGHT 2

namespace fe {
    class Renderer;
    class RenderStage;
    class CommandPool;
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

        /**
         * Takes a screenshot of the current image of the display and saves it into a image file.
         * @param filename The file to save the screenshot as.
         */
        void captureScreenshot(const std::filesystem::path& filename, size_t id = 0) const;

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

        const CommandPool* getCommandPool();
        const Surface* getSurface(size_t id) const { return surfaces[id].get(); }
        const Swapchain* getSwapchain(size_t id) const { return swapchains[id].get(); }

    private:
        struct FrameInfo {
            size_t id;
            size_t& currentFrame;
            Swapchain& swapchain;
            CommandBuffer& commandBuffer;
            SyncObject& syncObject;
        };
        bool beginFrame(FrameInfo& info);
        bool beginRenderpass(FrameInfo& info, RenderStage& renderStage);
        void endRenderpass(FrameInfo& info);
        void endFrame(FrameInfo& info);

        void resetRenderStages();
        void recreateSwapchain(size_t id);
        void recreateAttachmentsMap();

        void onWindowCreate(Window* window, bool create);

        Instance instance{};
        PhysicalDevice physicalDevice{ instance };
        LogicalDevice logicalDevice{ instance, physicalDevice };
        PipelineCache pipelineCache{ logicalDevice };

        std::map<std::string, const Descriptor*> attachments;

        std::unique_ptr<CommandPool> commandPool;
        std::unique_ptr<Renderer> renderer;

        std::vector<std::unique_ptr<Surface>> surfaces;
        std::vector<std::unique_ptr<Swapchain>> swapchains;

        struct PerSurfaceBuffers {
            size_t currentFrame{ 0 };
            std::vector<CommandBuffer> commandBuffers;
            std::vector<SyncObject> syncObjects;
        };
        std::vector<std::unique_ptr<PerSurfaceBuffers>> perSurfaceBuffers;
    };
}
