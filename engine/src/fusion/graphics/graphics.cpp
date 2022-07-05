#include "graphics.hpp"

#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/devices/devices.hpp"
#include "fusion/devices/window.hpp"
#include "fusion/graphics/renderpass/swapchain.hpp"
#include "fusion/graphics/renderpass/framebuffers.hpp"
#include "fusion/graphics/renderpass/renderpass.hpp"
#include "fusion/graphics/commands/command_pool.hpp"
#include "fusion/graphics/images/image_depth.hpp"
#include "fusion/graphics/render_stage.hpp"
#include "fusion/graphics/renderer.hpp"
#include "fusion/graphics/subrender.hpp"
#include "fusion/utils/enumerate.hpp"
#include "fusion/utils/date_time.hpp"

#include <glslang/Public/ShaderLang.h>

using namespace fe;

Graphics::Graphics() {
    for (auto& window : Devices::Get()->getWindows()) {
        onWindowCreate(window.get(), true);
    }
    Devices::Get()->OnWindowCreate().connect<&Graphics::onWindowCreate>(this);

    if (!glslang::InitializeProcess())
        throw std::runtime_error("Failed to initialize glslang process");
}

Graphics::~Graphics() {
    auto graphicsQueue = logicalDevice.getGraphicsQueue();

    swapchains.clear();

    Devices::Get()->OnWindowCreate().disconnect<&Graphics::onWindowCreate>(this);

    VK_CHECK(vkQueueWaitIdle(graphicsQueue));

    glslang::FinalizeProcess();

    perSurfaceBuffers.clear();

    commandPool = nullptr;
    renderer = nullptr;
}

void Graphics::onUpdate() {
    if (!renderer || Devices::Get()->getWindow(0)->isIconified())
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

        FrameInfo info {
            id,
            currentFrame,
            *swapchain,
            perSurfaceBuffer->commandBuffers[currentFrame],
            perSurfaceBuffer->syncObjects[currentFrame]
        };

        if (!beginFrame(info))
            continue;

        Pipeline::Stage stage;

        for (auto& renderStage : renderer->renderStages) {
            renderStage->update(id, *swapchain);

            if (!beginRenderpass(info, *renderStage))
                break;

            for (const auto& subpass : renderStage->getSubpasses()) {
                stage.second = subpass.binding;

                // Renders subpass subrender pipelines
                renderer->subrenderHolder.renderStage(stage, info.commandBuffer);

                if (subpass.binding != renderStage->getSubpasses().back().binding)
                    vkCmdNextSubpass(info.commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
            }

            endRenderpass(info);

            stage.first++;
        }

        endFrame(info);
    }
}

bool Graphics::beginFrame(FrameInfo& info) {
    auto& [id, frame, swapchain, commandBuffer, syncObject] = info;

    auto result = swapchain.acquireNextImage(syncObject.getImageAvailableSemaphore(), syncObject.getInFlightFence());

#ifndef PLATFORM_ANDROID
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain(id);
        return false;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }
#else
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }
#endif

    if (!commandBuffer.isRunning())
        commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    return true;
}

bool Graphics::beginRenderpass(FrameInfo& info, RenderStage& renderStage) {
    auto& [id, frame, swapchain, commandBuffer, syncObject] = info;

    if (renderStage.isOutOfDate()) {
        LOG_WARNING << "Render stage is out of date!";
        return false;
    }

    auto& renderArea = renderStage.getRenderArea();

    VkViewport viewport = vku::viewport(renderArea.extent);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = vku::rect2D(renderArea.extent, renderArea.offset);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    auto clearValues = renderStage.getClearValues();

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = *renderStage.getRenderpass();
    renderPassBeginInfo.framebuffer = renderStage.getActiveFramebuffer(swapchain.getActiveImageIndex());
    renderPassBeginInfo.renderArea = scissor; // same as render area
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    return true;
}

void Graphics::endRenderpass(FrameInfo& info) {
    vkCmdEndRenderPass(info.commandBuffer);
}

void Graphics::endFrame(FrameInfo& info) {
    auto& [id, frame, swapchain, commandBuffer, syncObject] = info;

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

#ifndef PLATFORM_ANDROID
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreateSwapchain(id);
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }
#else
    if (result != VK_SUCCESS) {
        LOG_WARNING << "Failed to present swap chain image!";
        VK_CHECK(result);
    }
#endif

    frame = (frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Graphics::captureScreenshot(const std::filesystem::path& filename, size_t id) const {
#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif

    const auto& size = Devices::Get()->getWindow(id)->getSize();

    VkImage dstImage;
    VkDeviceMemory dstImageMemory;
    //auto supportsBlit = Image::CopyImage(swapchains[id]->getActiveImage(), dstImage, dstImageMemory, surfaces[id]->getFormat().format, {size.x, size.y, 1}, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 0, 0);

    // Get layout of the image (including row pitch)
    VkImageSubresource imageSubresource = {};
    imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageSubresource.mipLevel = 0;
    imageSubresource.arrayLayer = 0;

    VkSubresourceLayout dstSubresourceLayout;
    vkGetImageSubresourceLayout(logicalDevice, dstImage, &imageSubresource, &dstSubresourceLayout);

    Bitmap bitmap{std::make_unique<uint8_t[]>(dstSubresourceLayout.size), size};

    void* data;
    vkMapMemory(logicalDevice, dstImageMemory, dstSubresourceLayout.offset, dstSubresourceLayout.size, 0, &data);
    memcpy(bitmap.getData<void>(), data, static_cast<size_t>(dstSubresourceLayout.size));
    vkUnmapMemory(logicalDevice, dstImageMemory);

    // Frees temp image and memory
    vkFreeMemory(logicalDevice, dstImageMemory, nullptr);
    vkDestroyImage(logicalDevice, dstImage, nullptr);

    // Writes the screenshot bitmap to the file
    bitmap.write(filename);

#if FUSION_DEBUG
    LOG_DEBUG << "Screenshot " << filename << " created in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}

const RenderStage* Graphics::getRenderStage(uint32_t index) const {
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
        auto& perSurfaceBuffer = perSurfaceBuffers.emplace_back(std::make_unique<PerSurfaceBuffers>());
        perSurfaceBuffer->commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        perSurfaceBuffer->syncObjects.resize(MAX_FRAMES_IN_FLIGHT);
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
    auto& size = surface->getWindow().getSize();
    LOG_DEBUG << "Recreating swapchain[" << id << "] old (" << swapchain->getExtent().width << ", " << swapchain->getExtent().height << ") new (" << size.x << ", " << size.y << ")";
#endif

    swapchain = std::make_unique<Swapchain>(physicalDevice, logicalDevice, *surface, swapchain.get());

    // Reset sync objects
    for (auto& sync : perSurfaceBuffer->syncObjects) {
        sync.reset();
    }

    auto graphicsQueue = logicalDevice.getGraphicsQueue();

    VK_CHECK(vkQueueWaitIdle(graphicsQueue));

    for (const auto& renderStage : renderer->renderStages)
        renderStage->rebuild(id, *swapchain);

    recreateAttachmentsMap();
}

void Graphics::recreateAttachmentsMap() {
    attachments.clear();

    for (const auto& renderStage : renderer->renderStages)
        attachments.insert(renderStage->descriptors.begin(), renderStage->descriptors.end());
}

void Graphics::onWindowCreate(Window* window, bool create) {
    if (!window) return;
    if (create) {
        auto& surface = surfaces.emplace_back(std::make_unique<Surface>(instance, physicalDevice, logicalDevice, *window));
    } else {
        /*surfaces.erase(std::remove_if(surfaces.begin(), surfaces.end(), [window](const auto& s) {
            return window == &s->window;
        }), surfaces.end());*/
    }
}

const CommandPool* Graphics::getCommandPool() {
    if (!commandPool) commandPool = std::make_unique<CommandPool>();
    return commandPool.get();
}