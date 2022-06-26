#include "graphics.hpp"
#include "subrender.hpp"

#include "fusion/utils/enumerate.hpp"
#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/devices/devices.hpp"
#include "fusion/devices/window.hpp"
#include "fusion/graphics/renderpass/fence.hpp"
#include "fusion/graphics/renderpass/semaphore.hpp"
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

Graphics::Graphics()
    : elapsedPurge(5s)
    , instance{}
    , physicalDevice{instance}
    , logicalDevice{instance, physicalDevice}
{
    for (auto& w : Devices::Get()->getWindows()) {
        onWindowCreate(w.get(), true);
    }
    Devices::Get()->OnWindowCreate().connect<&Graphics::onWindowCreate>(this);

    createPipelineCache();

    if (!glslang::InitializeProcess())
        throw std::runtime_error("Failed to initialize glslang process");
}

Graphics::~Graphics() {
    auto graphicsQueue = logicalDevice.getGraphicsQueue();

    swapchains.clear();

    Devices::Get()->OnWindowCreate().disconnect<&Graphics::onWindowCreate>(this);

    CheckVk(vkQueueWaitIdle(graphicsQueue));

    glslang::FinalizeProcess();

    vkDestroyPipelineCache(logicalDevice, pipelineCache, nullptr);

    perSurfaceBuffers.clear();
    commandPools.clear();

    renderer = nullptr;
}

void Graphics::update() {
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

        auto result = swapchain->acquireNextImage(perSurfaceBuffer->presentCompletes[currentFrame], perSurfaceBuffer->flightFences[currentFrame]);

#ifndef PLATFORM_ANDROID
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapchain(result);
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

            if (!startRenderpass(id, *renderStage))
                return;

            auto& commandBuffer = perSurfaceBuffer->commandBuffers[currentFrame];

            for (const auto& subpass : renderStage->getSubpasses()) {
                stage.second = subpass.binding;

                // Renders subpass subrender pipelines.
                renderer->subrenderHolder.renderStage(stage, commandBuffer);

                if (subpass.binding != renderStage->getSubpasses().back().binding)
                    vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
            }

            if (!endRenderpass(id, *renderStage))
                return;

            stage.first++;
        }
    }

    // Purges unused command pools.
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

void Graphics::captureScreenshot(const std::filesystem::path& filename, size_t id) const {
#if FUSION_DEBUG
    auto debugStart = Time::Now();
#endif

    const auto& size = Devices::Get()->getWindow(id)->getSize();

    VkImage dstImage;
    VkDeviceMemory dstImageMemory;
    //auto supportsBlit = Image::CopyImage(swapchains[id]->getActiveImage(), dstImage, dstImageMemory, surfaces[id]->getFormat().format, {size.x, size.y, 1}, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 0, 0);

    // Get layout of the image (including row pitch).
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

    // Frees temp image and memory.
    vkFreeMemory(logicalDevice, dstImageMemory, nullptr);
    vkDestroyImage(logicalDevice, dstImage, nullptr);

    // Writes the screenshot bitmap to the file.
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

const std::shared_ptr<CommandPool>& Graphics::getCommandPool(const std::thread::id &threadId) {
    if (auto it = commandPools.find(threadId); it != commandPools.end())
        return it->second;
    // TODO: Cleanup and fix crashes
    return commandPools.emplace(threadId, std::make_shared<CommandPool>(threadId)).first->second;
}

void Graphics::createPipelineCache() {
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    CheckVk(vkCreatePipelineCache(logicalDevice, &pipelineCacheCreateInfo, nullptr, &pipelineCache));
}

void Graphics::resetRenderStages() {
    recreateSwapchain(VK_EVENT_RESET);

    for (const auto& [id, swapchain] : enumerate(swapchains)) {
        for (const auto& renderStage : renderer->renderStages)
            renderStage->rebuild(id, *swapchain);
    }

    recreateAttachmentsMap();
}

void Graphics::recreateSwapchain(VkResult reason) {
    vkDeviceWaitIdle(logicalDevice);

    LOG_DEBUG << "Reason of recreating: " << StringifyResultVk(reason);

    swapchains.resize(surfaces.size());
    perSurfaceBuffers.resize(surfaces.size());
    for (const auto& [id, surface] : enumerate(surfaces)) {
        auto& window = surface->getWindow();
        auto& size = window.getSize();
        auto& swapchain = swapchains[id];
        if (swapchain) {
            LOG_DEBUG << "Recreating swapchain old (" << swapchain->getExtent().width << ", " << swapchain->getExtent().height << ") new (" << size.x << ", " << size.y << ")";
        } else {
            LOG_DEBUG << "Creating swapchain (" << size.x << ", " << size.y << ")";
        }
        swapchain = std::make_unique<Swapchain>(logicalDevice, *surface, vku::uvec2_cast(size), window.isVSync(), swapchain.get());
        perSurfaceBuffers[id] = std::make_unique<PerSurfaceBuffers>(swapchain->getImageCount());
    }
}

void Graphics::recreatePass(size_t idx, RenderStage& renderStage) {
    auto graphicsQueue = logicalDevice.getGraphicsQueue();
    CheckVk(vkQueueWaitIdle(graphicsQueue));

    for (const auto& [id, swapchain] : enumerate(swapchains)) {
        const auto& extent = vku::uvec2_cast(surfaces[id]->getWindow().getSize());
        while (extent.width == 0 || extent.height == 0) {
            Devices::Get()->waitEvents();
            LOG_DEBUG << "Wait to get extent. Current is unvalid " << "(" << extent.width << ", " << extent.height << ")";
        }

        if (renderStage.hasSwapchain() && !swapchain->isSameExtent(extent)) {
            recreateSwapchain(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        }
        renderStage.rebuild(id, *swapchain);
    }
    recreateAttachmentsMap();
}

void Graphics::recreateAttachmentsMap() {
    attachments.clear();

    for (const auto& renderStage : renderer->renderStages)
        attachments.insert(renderStage->descriptors.begin(), renderStage->descriptors.end());
}

bool Graphics::startRenderpass(size_t id, RenderStage& renderStage) {
    if (renderStage.isOutOfDate()) {
        recreatePass(id, renderStage);
        return false;
    }

    auto& swapchain = swapchains[id];
    auto& perSurfaceBuffer = perSurfaceBuffers[id];
    auto& currentFrame = perSurfaceBuffer->currentFrame;
    auto& commandBuffer = perSurfaceBuffer->commandBuffers[currentFrame];

    if (!commandBuffer.isRunning())
        commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

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
    auto presentQueue = logicalDevice.getPresentQueue();
    auto& swapchain = swapchains[id];
    auto& perSurfaceBuffer = perSurfaceBuffers[id];
    auto& currentFrame = perSurfaceBuffer->currentFrame;
    auto& commandBuffer = perSurfaceBuffer->commandBuffers[currentFrame];

    vkCmdEndRenderPass(commandBuffer);

    if (!renderStage.hasSwapchain())
        return false;

    commandBuffer.end();

    auto imageIndex = swapchain->getActiveImageIndex();

    commandBuffer.submit(perSurfaceBuffer->presentCompletes[currentFrame], perSurfaceBuffer->renderCompletes[currentFrame], perSurfaceBuffer->flightFences[currentFrame]);

    auto presentResult = swapchain->queuePresent(presentQueue, perSurfaceBuffer->renderCompletes[currentFrame]);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

#ifndef PLATFORM_ANDROID
    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
        recreateSwapchain(presentResult);
        return false;
    } else if (presentResult != VK_SUCCESS) {
        LOG_ERROR << "Failed to present swap chain image!";
        CheckVk(presentResult);
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
        surfaces.push_back(std::make_unique<Surface>(instance, physicalDevice, logicalDevice, *window));
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

Graphics::PerSurfaceBuffers::PerSurfaceBuffers(size_t imageCount) {
    commandBuffers.resize(imageCount);
    presentCompletes.resize(imageCount);
    renderCompletes.resize(imageCount);
    flightFences.resize(imageCount);
}
