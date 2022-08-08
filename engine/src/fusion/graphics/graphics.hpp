#pragma once

#include "fusion/devices/window.hpp"
#include "fusion/graphics/renderer.hpp"
#include "fusion/graphics/devices/instance.hpp"
#include "fusion/graphics/devices/logical_device.hpp"
#include "fusion/graphics/devices/physical_device.hpp"
#include "fusion/graphics/devices/surface.hpp"
#include "fusion/graphics/pipelines/pipeline_cache.hpp"
#include "fusion/graphics/renderpass/sync_object.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/graphics/descriptors/descriptor_allocator.hpp"
#include "fusion/graphics/descriptors/descriptor_layout_cache.hpp"
#include "fusion/graphics/pipelines/pipeline_layout_cache.hpp"
#include "fusion/graphics/textures/sampler_cache.hpp"

#define MAX_FRAMES_IN_FLIGHT 2

namespace tracy {
    class VkCtx;
}

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

        /**
         * Gets the current renderer.
         * @return The renderer.
         */
        Renderer* getRenderer() const { return renderer.get(); }

        /**
         * Sets the current renderer to a new renderer.
         * @param renderer The new renderer.
         */
        void setRenderer(std::unique_ptr<Renderer>&& rend) { renderer = std::move(rend); }

        RenderStage* getRenderStage(size_t index) const;
        const Descriptor* getAttachment(const std::string& name) const;

        const PhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
        const LogicalDevice& getLogicalDevice() const { return logicalDevice; }
        const VkPipelineCache& getPipelineCache() const { return pipelineCache; }
        const SamplerCache& getSamplerCache() const { return samplerCache; }
        const DescriptorAllocator& getDescriptorAllocator() const { return descriptorAllocator; }
        const DescriptorLayoutCache& getDescriptorLayoutCache() const { return descriptorLayoutCache; }
        const PipelineLayoutCache& getPipilineLayoutCache() const { return pipelineLayoutCache; }
        const std::shared_ptr<CommandPool>& getCommandPool(const std::thread::id& threadId = std::this_thread::get_id());
        const Surface* getSurface(size_t id) const { return surfaces[id].get(); }
        const Swapchain* getSwapchain(size_t id) const { return swapchains[id].get(); }

        size_t getCurrentFrame(size_t id) const { return perSurfaceBuffers[id]->currentFrame; }

        /**
         * Takes a screenshot of the current image of the display and saves it into a image file.
         * @param filepath The file to save the screenshot as.
         */
        void captureScreenshot(const fs::path& filepath, size_t id = 0) const;


    private:
        void onUpdate() override;
        void onStop() override;

        struct FrameInfo {
            const size_t& id;
            size_t& currentFrame;
            Swapchain& swapchain;
            CommandBuffer& commandBuffer;
            SyncObject& syncObject;
#if FUSION_PROFILE && TRACY_ENABLE
            tracy::VkCtx* tracyContext;
#endif
        };

#if FUSION_PROFILE && TRACY_ENABLE
        #define UNPACK_FRAME_INFO(info) auto& [id, currentFrame, swapchain, commandBuffer, syncObject, tracyContext] = info
        #define FUSION_PROFILE_GPU(name) TracyVkZone(tracyContext, commandBuffer, name)
#else
        #define UNPACK_FRAME_INFO(info) auto& [id, currentFrame, swapchain, commandBuffer, syncObject] = info
        #define FUSION_PROFILE_GPU(name)
#endif
        bool beginFrame(FrameInfo& info);
        bool beginRenderpass(FrameInfo& info, RenderStage& renderStage);
        void nextSubpasses(FrameInfo& info, RenderStage& renderStage, Pipeline::Stage& pipelineStage);
        void endRenderpass(FrameInfo& info);
        void endFrame(FrameInfo& info);

        void resetRenderStages();
        void recreateSwapchain(size_t id);
        void recreateAttachmentsMap();
        void recreatePass(FrameInfo& info, RenderStage& renderStage);

        void onWindowCreate(Window* window, bool create);

        Instance instance{};
        PhysicalDevice physicalDevice{ instance };
        LogicalDevice logicalDevice{ instance, physicalDevice };

        DescriptorAllocator descriptorAllocator{ logicalDevice };
        DescriptorLayoutCache descriptorLayoutCache{ logicalDevice };

        SamplerCache samplerCache{ logicalDevice };
        PipelineCache pipelineCache{ logicalDevice };
        PipelineLayoutCache pipelineLayoutCache{ logicalDevice };

        std::flat_map<std::string, const Descriptor*> attachments;

        std::unordered_map<std::thread::id, std::shared_ptr<CommandPool>> commandPools;
        ElapsedTime elapsedPurge; /// Timer used to remove unused command pools.
        std::unique_ptr<Renderer> renderer;

        std::vector<std::unique_ptr<Surface>> surfaces;
        std::vector<std::unique_ptr<Swapchain>> swapchains;

        struct PerSurfaceBuffers {
            PerSurfaceBuffers();
            ~PerSurfaceBuffers();
            size_t currentFrame{ 0 };
            std::vector<CommandBuffer> commandBuffers;
            std::vector<SyncObject> syncObjects;
#if FUSION_PROFILE && TRACY_ENABLE
            std::vector<tracy::VkCtx*> tracyContexts;
#endif
        };
        std::vector<std::unique_ptr<PerSurfaceBuffers>> perSurfaceBuffers;
    };
}
