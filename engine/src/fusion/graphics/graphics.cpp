#include "graphics.hpp"
#include "subrender.hpp"

#include "fusion/utils/enumerate.hpp"
#include "fusion/utils/time.hpp"
#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/devices/devices.hpp"
#include "fusion/devices/window.hpp"
#include "fusion/graphics/renderpass/sync_object.hpp"
#include "fusion/graphics/renderpass/swapchain.hpp"
#include "fusion/graphics/renderpass/framebuffers.hpp"
#include "fusion/graphics/renderpass/renderpass.hpp"
#include "fusion/graphics/images/image_depth.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/graphics/commands/command_pool.hpp"
#include "fusion/graphics/render_stage.hpp"
#include "fusion/graphics/renderer.hpp"

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

    CheckVk(vkQueueWaitIdle(graphicsQueue));

    glslang::FinalizeProcess();

    perSurfaceBuffers.clear();

    commandPool = nullptr;
    renderer = nullptr;
}

void Graphics::update(const Time& dt) {
    if (!renderer || Devices::Get()->getWindow(0)->isIconified())
        return;

    if (!renderer->started) {
        resetRenderStages();
        renderer->start();
        renderer->started = true;
    }

    renderer->update();

    for (const auto& [id, swapchain] : enumerate(swapchains)) {
        auto& perSurfaceBuffer = perSurfaceBuffers[id];
        auto& currentFrame = perSurfaceBuffer->currentFrame;
        auto& syncObject = perSurfaceBuffer->syncObjects[currentFrame];

        auto result = swapchain->acquireNextImage(syncObject.getImageAvailableSemaphore(), syncObject.getInFlightFence());

#ifndef PLATFORM_ANDROID
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapchain(id);
            return;
        }
#endif
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            LOG_ERROR << "Failed to acquire swap chain image!";
            return;
        }

        Pipeline::Stage stage;

        for (auto& renderStage : renderer->renderStages) {
            renderStage->update(id);

            startRenderpass(id, *renderStage);

            auto& commandBuffer = perSurfaceBuffer->commandBuffers[currentFrame];

            for (const auto& subpass : renderStage->getSubpasses()) {
                stage.second = subpass.binding;

                // Renders subpass subrender pipelines
                renderer->subrenderHolder.renderStage(stage, commandBuffer);

                if (subpass.binding != renderStage->getSubpasses().back().binding)
                    vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
            }

            if (!endRenderpass(id, *renderStage))
                break;

            stage.first++;
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}

void Graphics::captureScreenshot(const std::filesystem::path& filename, size_t id) const {
#if FUSION_DEBUG
    auto debugStart = Time::Now();
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
    LOG_DEBUG << "Screenshot " << filename << " created in " << (Time::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}

const RenderStage* Graphics::getRenderStage(uint32_t index) const {
    if (renderer)
        return renderer->getRenderStage(index);
    return nullptr;
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
    CheckVk(vkDeviceWaitIdle(logicalDevice));

    auto& swapchain = swapchains[id];
    auto& surface = surfaces[id];
    auto& perSurfaceBuffer = perSurfaceBuffers[id];

#if FUSION_DEBUG
    auto& window = surface->getWindow();
    auto& size = window.getSize();
    if (swapchain) {
        LOG_DEBUG << "Recreating swapchain[" << id << "] old (" << swapchain->getExtent().width << ", " << swapchain->getExtent().height << ") new (" << size.x << ", " << size.y << ")";
    } else {
        LOG_DEBUG << "Creating swapchain[" << id << "] (" << size.x << ", " << size.y << ")";
    }
#endif

    swapchain = std::make_unique<Swapchain>(physicalDevice, logicalDevice, *surface, swapchain.get());
    // Reset sync objects
    for (auto& sync : perSurfaceBuffer->syncObjects) {
        sync.reset();
    }

    auto graphicsQueue = logicalDevice.getGraphicsQueue();
    CheckVk(vkQueueWaitIdle(graphicsQueue));

    for (const auto& renderStage : renderer->renderStages)
        renderStage->rebuild(id, *swapchain);

    recreateAttachmentsMap();
}

void Graphics::recreateAttachmentsMap() {
    attachments.clear();

    for (const auto& renderStage : renderer->renderStages)
        attachments.insert(renderStage->descriptors.begin(), renderStage->descriptors.end());
}

bool Graphics::startRenderpass(size_t id, RenderStage& renderStage) {
    if (renderStage.isOutOfDate()) {
        recreateSwapchain(id);
        return false;
    }

    auto& swapchain = swapchains[id];
    auto& perSurfaceBuffer = perSurfaceBuffers[id];
    auto& currentFrame = perSurfaceBuffer->currentFrame;
    auto& commandBuffer = perSurfaceBuffer->commandBuffers[currentFrame];

    if (!commandBuffer.isRunning())
        commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT); //VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT

    auto& renderArea = renderStage.getRenderArea();

    VkViewport viewport = vku::viewport(renderArea.extent);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = vku::rect2D(renderArea.extent, renderArea.offset);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    auto clearValues = renderStage.getClearValues();

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = *renderStage.getRenderpass();
    renderPassBeginInfo.framebuffer = renderStage.getActiveFramebuffer(swapchain->getActiveImageIndex());
    renderPassBeginInfo.renderArea = scissor; // same as render area
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    return true;
}

bool Graphics::endRenderpass(size_t id, RenderStage& renderStage) {
    auto& swapchain = swapchains[id];
    auto& perSurfaceBuffer = perSurfaceBuffers[id];
    auto& currentFrame = perSurfaceBuffer->currentFrame;
    auto& commandBuffer = perSurfaceBuffer->commandBuffers[currentFrame];
    auto& syncObject = perSurfaceBuffer->syncObjects[currentFrame];

    vkCmdEndRenderPass(commandBuffer);

    if (!renderStage.hasSwapchain()) {
        throw std::runtime_error("Render stage is not have swapchain");
    }

    commandBuffer.end();

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (syncObject.getImageInFlightFence() != VK_NULL_HANDLE) {
        vkWaitForFences(logicalDevice, 1, &syncObject.getImageInFlightFence(), VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    syncObject.setImageInUse();

    commandBuffer.submit(syncObject.getImageAvailableSemaphore(), syncObject.getRenderFinishedSemaphore(), syncObject.getInFlightFence());

    auto presentQueue = logicalDevice.getPresentQueue();
    auto result = swapchain->queuePresent(presentQueue, syncObject.getRenderFinishedSemaphore());

#ifndef PLATFORM_ANDROID
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreateSwapchain(id);
        return false;
    } else if (result != VK_SUCCESS) {
        LOG_ERROR << "Failed to present swap chain image!";
        CheckVk(result);
        return false;
    }
#else
    if (presentResult != VK_SUCCESS) {
        LOG_ERROR << "Failed to present swap chain image!";
        CheckVk(presentResult);
        return false;
    }
#endif

    return true;
}

void Graphics::onWindowCreate(Window* window, bool create) {
    if (!window) return;
    if (create) {
        auto& surface = surfaces.emplace_back(std::make_unique<Surface>(instance, physicalDevice, logicalDevice, *window));

        /*auto& swapchain = swapchains.emplace_back(std::make_unique<Swapchain>(physicalDevice, logicalDevice, *surface, nullptr));
        auto& perSurfaceBuffer = perSurfaceBuffers.emplace_back();
        perSurfaceBuffer->commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        perSurfaceBuffer->syncObjects.resize(MAX_FRAMES_IN_FLIGHT);
        for (const auto& renderStage : renderer->renderStages)
            renderStage->rebuild(surfaces.size() - 1, *swapchain);
        recreateAttachmentsMap();*/
    } else {
        /*surfaces.erase(std::remove_if(surfaces.begin(), surfaces.end(), [window](const auto& s) {
            return window == &s->window;
        }), surfaces.end());*/
    }
}

std::string Graphics::StringifyResultVk(VkResult result) {
    switch (result) {
        case VK_SUCCESS:
            return "Success";
        case VK_NOT_READY:
            return "A fence or query has not yet completed";
        case VK_TIMEOUT:
            return "A wait operation has not completed in the specified time";
        case VK_EVENT_SET:
            return "An event is signaled";
        case VK_EVENT_RESET:
            return "An event is unsignaled";
        case VK_INCOMPLETE:
            return "A return array was too small for the result";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "A host memory allocation has failed";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "A device memory allocation has failed";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "Initialization of an object could not be completed for implementation-specific reasons";
        case VK_ERROR_DEVICE_LOST:
            return "The logical or physical device has been lost";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "Mapping of a memory object has failed";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "A requested layer is not present or could not be loaded";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "A requested extension is not supported";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "A requested feature is not supported";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "Too many objects of the type have already been created";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "A requested format is not supported on this device";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "A surface is no longer available";
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return "A allocation failed due to having no more space in the descriptor pool";
        case VK_SUBOPTIMAL_KHR:
            return "A swapchain no longer matches the surface properties exactly, but can still be used";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "A surface has changed in such a way that it is no longer compatible with the swapchain";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "The display used by a swapchain does not use the same presentable image layout";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "A validation layer found an error";
        default:
            return "Unknown Vulkan error";
    }
}

void Graphics::CheckVk(VkResult result) {
    if (result >= 0) return;
    auto failure = StringifyResultVk(result);
    throw std::runtime_error("Vulkan error: " + failure);
}

const CommandPool* Graphics::getCommandPool() {
    if (!commandPool) commandPool = std::make_unique<CommandPool>();
    return commandPool.get();
}
