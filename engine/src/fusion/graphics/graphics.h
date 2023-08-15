#pragma once

#include "fusion/devices/window.h"
#include "fusion/graphics/renderer.h"
#include "fusion/graphics/devices/instance.h"
#include "fusion/graphics/devices/logical_device.h"
#include "fusion/graphics/devices/physical_device.h"
#include "fusion/graphics/devices/surface.h"
#include "fusion/graphics/pipelines/pipeline_cache.h"
#include "fusion/graphics/renderpass/sync_object.h"
#include "fusion/graphics/commands/command_buffer.h"
#include "fusion/graphics/descriptors/descriptor_allocator.h"
#include "fusion/graphics/descriptors/descriptor_layout_cache.h"
#include "fusion/graphics/pipelines/pipeline_layout_cache.h"
#include "fusion/graphics/textures/sampler_cache.h"

#include <thread>

static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
static const uint32_t MAX_BINDLESS_RESOURCES = 1024;

namespace tracy {
    class VkCtx;
}

namespace fe {
    template<typename T>
    class Module;
    /**
     * @brief Module that manages the Vulkan's graphics context.
     */
    class FUSION_API Graphics {
        friend class Module<Graphics>;
    private:
        Graphics();
        ~Graphics();

    public:
        static Graphics* Get() { return Instance; }

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

        const PipelineCache& getPipelineCache() const { return pipelineCache; }
        const SamplerCache& getSamplerCache() const { return samplerCache; }
        const DescriptorLayoutCache& getDescriptorLayoutCache() const { return descriptorLayoutCache; }
        const PipelineLayoutCache& getPipilineLayoutCache() const { return pipelineLayoutCache; }
        const DescriptorAllocator& getDescriptorAllocator() const { return descriptorAllocator; }
        const DescriptorAllocator& getIndexedDescriptorAllocator() const { return indexedDescriptorAllocator; }

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
        void onStart();
        void onUpdate();
        void onStop();

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

        DescriptorLayoutCache descriptorLayoutCache{ logicalDevice };
        DescriptorAllocator descriptorAllocator{ logicalDevice, 1024, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT };
        DescriptorAllocator indexedDescriptorAllocator{ logicalDevice, 1024, VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT };

        SamplerCache samplerCache{ logicalDevice };
        PipelineCache pipelineCache{ logicalDevice };
        PipelineLayoutCache pipelineLayoutCache{ logicalDevice };

        fst::unordered_flatmap<std::string, const Descriptor*> attachments;

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

        static Graphics* Instance;
    };
}
