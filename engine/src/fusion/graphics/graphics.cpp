#include "graphics.h"

#include "fusion/bitmaps/bitmap.h"
#include "fusion/devices/device_manager.h"
#include "fusion/devices/window.h"
#include "fusion/graphics/renderpass/swapchain.h"
#include "fusion/graphics/renderpass/framebuffers.h"
#include "fusion/graphics/renderpass/renderpass.h"
#include "fusion/graphics/commands/command_pool.h"
#include "fusion/graphics/textures/texture_depth.h"
#include "fusion/graphics/render_stage.h"
#include "fusion/graphics/renderer.h"
#include "fusion/graphics/subrender.h"

#include <glslang/Public/ShaderLang.h>

#if FUSION_PROFILE && TRACY_ENABLE
#include <tracy/TracyVulkan.hpp>
#endif

using namespace fe;

Graphics* Graphics::Instance = nullptr;

Graphics::Graphics() : elapsedPurge{5s} {
    Instance = this;

    /*for (auto& window : DeviceManager::Get()->getWindows()) {
        onWindowCreate(window.get(), true);
    }*/
    DeviceManager::Get()->OnWindowCreate().connect<&Graphics::onWindowCreate>(this);

    if (!glslang::InitializeProcess())
        throw std::runtime_error("Failed to initialize glslang process");
}

Graphics::~Graphics() {
    Instance = nullptr;

    auto graphicsQueue = logicalDevice.getGraphicsQueue();

    swapchains.clear();

    //DeviceManager::Get()->OnWindowCreate().disconnect<&Graphics::onWindowCreate>(this);

    VK_CHECK(vkQueueWaitIdle(graphicsQueue));

    glslang::FinalizeProcess();

    perSurfaceBuffers.clear();
    commandPools.clear();

    renderer.reset();
}

void Graphics::onStop() {
    VK_CHECK(vkDeviceWaitIdle(logicalDevice));
}

void Graphics::onStart() {
}

void Graphics::onUpdate() {
    if (!renderer || DeviceManager::Get()->getWindow(0)->isIconified())
        return;

    if (!renderer->started) {
        resetRenderStages();
        renderer->onStart();
        renderer->started = true;
    }

    renderer->onUpdate();
    renderer->subrenderHolder.updateAll();

    for (const auto& [id, swapchain] : enumerate(swapchains)) {
        auto& perSurfaceBuffer = perSurfaceBuffers[id];
        auto& currentFrame = perSurfaceBuffer->currentFrame;

#if FUSION_PROFILE && TRACY_ENABLE
#define info perSurfaceBuffer->commandBuffers[currentFrame], perSurfaceBuffer->syncObjects[currentFrame], perSurfaceBuffer->tracyContexts[currentFrame], *swapchain, currentFrame, id
#else
#define info perSurfaceBuffer->commandBuffers[currentFrame], perSurfaceBuffer->syncObjects[currentFrame], *swapchain, currentFrame, id
#endif

        if (!beginFrame(info))
            continue;

        Pipeline::Stage stage;

        for (auto& renderStage : renderer->renderStages) {
            renderStage->update(id, *swapchain);

            if (beginRenderpass(info, *renderStage)) {
                nextSubpasses(info, *renderStage, stage);
                endRenderpass(info);
            }

            ++stage.first;
        }

        endFrame(info);
    }

    if (elapsedPurge.getElapsed() != 0) {
        for (auto it = commandPools.begin(); it != commandPools.end();) {
            if ((*it).second.use_count() <= 1) {
                it = commandPools.erase(it);
                continue;
            }
            ++it;
        }
    }
}

bool Graphics::beginFrame(FRAME_INFO) {
    auto result = swapchain.acquireNextImage(syncObject.getImageAvailableSemaphore(), syncObject.getInFlightFence());

#if FUSION_PLATFORM_ANDROID
    if (result != VK_SUCCESS)
        throw std::runtime_error("Failed to acquire swap chain image!");
#else
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain(id);
        return false;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("Failed to acquire swap chain image!");
#endif

    FE_ASSERT(!commandBuffer.isRunning());

    commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    FUSION_PROFILE_GPU("Begin Frame");

    return true;
}

bool Graphics::beginRenderpass(FRAME_INFO, RenderStage& renderStage) {
    FUSION_PROFILE_GPU("Begin Renderpass");

    if (renderStage.isOutOfDate()) {
        //FE_LOG_WARNING("Render stage is out of date!");
        recreatePass(id, swapchain, renderStage);
        return false;
    }

    const auto& renderArea = renderStage.getRenderArea();

    VkViewport viewport{ vku::viewport(renderArea.extent) };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{ vku::rect2D(renderArea.extent, renderArea.offset) };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    auto clearValues = renderStage.getClearValues();

    VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    renderPassBeginInfo.renderPass = *renderStage.getRenderpass();
    renderPassBeginInfo.framebuffer = renderStage.getActiveFramebuffer(swapchain.getActiveImageIndex());
    renderPassBeginInfo.renderArea = scissor; // same as render area
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    return true;
}

void Graphics::nextSubpasses(FRAME_INFO, RenderStage& renderStage, Pipeline::Stage& pipelineStage) {
    auto lastBinding = renderStage.getSubpasses().back().binding;

    for (const auto& subpass : renderStage.getSubpasses()) {
        FUSION_PROFILE_GPU("Begin Subpass");

        pipelineStage.second = subpass.binding;

        // Renders subpass subrender pipelines
        renderer->subrenderHolder.renderStage(pipelineStage, commandBuffer, renderStage.getOverrideCamera());

        if (subpass.binding != lastBinding)
            vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
    }
}

void Graphics::endRenderpass(FRAME_INFO) {
    vkCmdEndRenderPass(commandBuffer);
}

void Graphics::endFrame(FRAME_INFO) {
#if FUSION_PROFILE && TRACY_ENABLE
    TracyVkCollect(tracyContext, commandBuffer);
#endif

    commandBuffer.end();

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (syncObject.getImageInFlightFence() != VK_NULL_HANDLE) {
        vkWaitForFences(logicalDevice, 1, &syncObject.getImageInFlightFence(), VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    syncObject.setImageInUse();

    commandBuffer.submit(syncObject.getImageAvailableSemaphore(), syncObject.getRenderFinishedSemaphore(), syncObject.getInFlightFence());

    auto presentQueue = logicalDevice.getPresentQueue();
    auto result = swapchain.queuePresent(presentQueue, syncObject.getRenderFinishedSemaphore());

#if FUSION_PLATFORM_ANDROID
    VK_CHECK_RESULT(result);
#else
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreateSwapchain(id);
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }
#endif

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Graphics::captureScreenshot(const fs::path& filepath, size_t id) const {
#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif

    const auto& size = DeviceManager::Get()->getWindow(id)->getSize();
    auto& swapchain = swapchains[id];
    VkFormat format = swapchain->getSurfaceFormat().format;

    VkImage dstImage;
    VkDeviceMemory dstImageMemory;

    bool supportsBlit = Image::CopyImage(swapchain->getActiveImage(), dstImage, dstImageMemory, format, VK_FORMAT_R8G8B8A8_UNORM, { size.x, size.y, 1 }, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 0, 0);

    // Get layout of the image (including row pitch)
    VkImageSubresource imageSubresource = {};
    imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageSubresource.mipLevel = 0;
    imageSubresource.arrayLayer = 0;

    VkSubresourceLayout dstSubresourceLayout;
    vkGetImageSubresourceLayout(logicalDevice, dstImage, &imageSubresource, &dstSubresourceLayout);

    Bitmap bitmap{std::make_unique<uint8_t[]>(dstSubresourceLayout.size), size};

    // If source is BGR (destination is always RGB) and we can't use blit (which does automatic conversion), we'll have to manually swizzle color components
    bool colorSwizzle = false;

    // Check if source is BGR
    // Note: Not complete, only contains most common and basic BGR surface formats for demonstration purposes
    if (!supportsBlit) {
        switch (format) {
            case VK_FORMAT_B8G8R8A8_SRGB:
            case VK_FORMAT_B8G8R8A8_UNORM:
            case VK_FORMAT_B8G8R8A8_SNORM:
                colorSwizzle = true;
            default:
                break;
        }
    }

    void* data;
    vkMapMemory(logicalDevice, dstImageMemory, dstSubresourceLayout.offset, dstSubresourceLayout.size, 0, &data);
    if (colorSwizzle)
        vku::rgba_to_bgra(bitmap.getData<uint8_t>(), static_cast<const uint8_t*>(data), size.x * size.y);
    else
        std::memcpy(bitmap.getData<void>(), data, dstSubresourceLayout.size);
    vkUnmapMemory(logicalDevice, dstImageMemory);

    // Frees temp image and memory
    vkFreeMemory(logicalDevice, dstImageMemory, nullptr);
    vkDestroyImage(logicalDevice, dstImage, nullptr);

    // Writes the screenshot bitmap to the file
    bitmap.write(filepath);

#if FUSION_DEBUG
    FE_LOG_DEBUG("Screenshot '{}' created in {}ms", filepath, (DateTime::Now() - debugStart).asMilliseconds<float>());
#endif
}

RenderStage* Graphics::getRenderStage(size_t index) const {
    return renderer ? renderer->getRenderStage(index) : nullptr;
}

const Descriptor* Graphics::getAttachment(const std::string& name) const {
    if (auto it = attachments.find(name); it != attachments.end())
        return it->second;
    return nullptr;
}

void Graphics::resetRenderStages() {
    for (const auto& [id, surface] : enumerate(surfaces)) {
        auto& swapchain = swapchains.emplace_back(std::make_unique<Swapchain>(physicalDevice, logicalDevice, *surface, nullptr));
        perSurfaceBuffers.push_back(std::make_unique<PerSurfaceBuffers>());
        for (const auto& renderStage : renderer->renderStages)
            renderStage->rebuild(id, *swapchain);
    }

    recreateAttachmentsMap();
}

void Graphics::recreateSwapchain(size_t id) {
    VK_CHECK(vkDeviceWaitIdle(logicalDevice));

    auto& swapchain = swapchains[id];
    auto& surface = surfaces[id];
    auto& perSurfaceBuffer = perSurfaceBuffers[id];

#if FUSION_DEBUG
    //auto& size = surface->getWindow().getSize();
    //FE_LOG_DEBUG("Recreating swapchain[" << id << "] old (" << swapchain->getExtent().width << ", " << swapchain->getExtent().height << ") new (" << size.x << ", " << size.y << ")");
#endif
    swapchain = std::make_unique<Swapchain>(physicalDevice, logicalDevice, *surface, swapchain.get());

    // Reset sync objects
    for (auto& sync : perSurfaceBuffer->syncObjects)
        sync.reset();

    auto graphicsQueue = logicalDevice.getGraphicsQueue();
    VK_CHECK(vkQueueWaitIdle(graphicsQueue));

    for (const auto& renderStage : renderer->renderStages)
        renderStage->rebuild(id, *swapchain);

    recreateAttachmentsMap();
}

void Graphics::recreateAttachmentsMap() {
    attachments.clear();

    for (const auto& renderStage : renderer->renderStages) {
        for (const auto& [name, descriptor] : renderStage->descriptors) {
            attachments.emplace(name, descriptor);
        }
    }
}

void Graphics::recreatePass(size_t id, Swapchain& swapchain, RenderStage& renderStage) {
    // Swapchain should recreate in the begin or the end frame stage
    if (renderStage.hasSwapchain())
        return;

    auto graphicsQueue = logicalDevice.getGraphicsQueue();
    VK_CHECK(vkQueueWaitIdle(graphicsQueue));

    renderStage.rebuild(id, swapchain);
    recreateAttachmentsMap();
}

void Graphics::onWindowCreate(Window* window, bool create) {
    if (!window) return;
    if (create) {
        surfaces.push_back(std::make_unique<Surface>(instance, physicalDevice, *window));
    } else {
        auto it = std::find_if(surfaces.begin(), surfaces.end(), [window](const auto& s) {
            return window == &s->window;
        });
        if (it != surfaces.end())
            surfaces.erase(it);
    }
}

const std::shared_ptr<CommandPool>& Graphics::getCommandPool(const std::thread::id& threadId) {
    if (auto it = commandPools.find(threadId); it != commandPools.end())
        return it->second;
    return commandPools.emplace(threadId, std::make_shared<CommandPool>(threadId)).first->second;
}

Graphics::PerSurfaceBuffers::PerSurfaceBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    syncObjects.resize(MAX_FRAMES_IN_FLIGHT);
#if FUSION_PROFILE && TRACY_ENABLE
    tracyContexts.resize(MAX_FRAMES_IN_FLIGHT);

    CommandBuffer tracyBuffer{false};

    const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    auto graphicsQueue = logicalDevice.getGraphicsQueue();

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        tracyContexts[i] = TracyVkContext(physicalDevice, logicalDevice, graphicsQueue, tracyBuffer);

    VK_CHECK(vkQueueWaitIdle(graphicsQueue));
#endif
}

Graphics::PerSurfaceBuffers::~PerSurfaceBuffers() {
#if FUSION_PROFILE && TRACY_ENABLE
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        TracyVkDestroy(tracyContexts[i]);
#endif
}