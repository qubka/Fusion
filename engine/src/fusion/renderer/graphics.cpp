#include "graphics.hpp"

#if defined(__ANDROID__)
#include "fusion/renderer/android/window.hpp"
#else
#include "fusion/renderer/glfw/window.hpp"
#endif

using namespace fe;

Graphics::Graphics() {
#if defined(__ANDROID__)
    window = std::make_unique<android::Window>();
#else
    window = std::make_unique<glfw::Window>("Fusion", glm::ivec2{ 1280, 720 });
#endif

    // TODO make this less stupid
    context.setDeviceFeaturesPicker([&](const vk::PhysicalDevice& device, vk::PhysicalDeviceFeatures2& features){
        if (context.deviceFeatures.textureCompressionBC) {
            context.enabledFeatures.textureCompressionBC = VK_TRUE;
        } else if (context.deviceFeatures.textureCompressionASTC_LDR) {
            context.enabledFeatures.textureCompressionASTC_LDR = VK_TRUE;
        } else if (context.deviceFeatures.textureCompressionETC2) {
            context.enabledFeatures.textureCompressionETC2 = VK_TRUE;
        }
        if (context.deviceFeatures.samplerAnisotropy) {
            context.enabledFeatures.samplerAnisotropy = VK_TRUE;
        }
    });

#if defined(__ANDROID__)
    context.requireExtensions({ VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME });
#else
    context.requireExtensions(glfw::Window::getRequiredInstanceExtensions());
#endif
    context.requireDeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });
    context.createInstance(VK_MAKE_VERSION(1, 1, 0));

#if defined(__ANDROID__)
    auto surface = context.instance.createAndroidSurfaceKHR({ {}, android::androidApp->window });
#else
    auto surface = reinterpret_cast<glfw::Window*>(window.get())->createSurface(context.instance);
#endif

    context.createDevice(surface);

    //temp
    recreateSwapchain();
}

Graphics::~Graphics() {
    renderer = nullptr;

    context.graphicsQueue.waitIdle();

    for (size_t i = 0; i < flightFences.size(); i++) {
        context.device.destroyFence(flightFences[i]);
        context.device.destroy(renderCompletes[i]);
        context.device.destroy(presentCompletes[i]);
    }

    context.device.freeCommandBuffers(context.getCommandPool(), commandBuffers);

    swapchain.destroy();
    context.destroy();
}

void Graphics::update() {
    if (!renderer || window->isMinimized()) return;

    if (!renderer->started) {
        resetRenderStages();
        renderer->onStart();
        renderer->started = true;
    }

    renderer->onUpdate();

    auto result = swapchain.acquireNextImage(presentCompletes[currentFrame], flightFences[currentFrame]);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapchain();
        return;
    }

    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire next image: " + vk::to_string(result));
        return;
    }

    /*Pipeline::Stage stage;

    for (auto& renderStage : renderer->renderStages) {
        renderStage->update();

        if (!startRenderpass(*renderStage))
            return;

        auto& commandBuffer = commandBuffers[swapchain.currentImage];

        for (const auto& subpass : renderStage->subpasses) {
            stage.second = subpass.binding;

            // Renders subpass subrender pipelines.
            renderer->subrenderHolder.renderStage(stage, *commandBuffer);

            if (subpass.binding != renderStage->subpasses.back().binding)
                commandBuffer.nextSubpass(vk::SubpassContents::eInline);
        }

        endRenderpass(*renderStage);
        stage.first++;
    }

    // Purges unused command pools.
    if (elapsedPurge.GetElapsed() != 0) {
        for (auto it = commandPools.begin(); it != commandPools.end();) {
            if ((*it).second.use_count() <= 1) {
                it = commandPools.erase(it);
                continue;
            }
            ++it;
        }
    }*/

    RenderStage

    startRenderpass();
    endRenderpass();
}

void Graphics::resetRenderStages() {
    recreateSwapchain();

    if (flightFences.size() != swapchain.imageCount)
        recreateCommandBuffers();

    for (const auto &renderStage : renderer->renderStages)
        renderStage->rebuild(swapchain);

    recreateAttachmentsMap();
}

void Graphics::recreateSwapchain() {
    auto extent = window->getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = window->getExtent();
        window->waitEvents();
    }

    context.device.waitIdle();

    LOG_DEBUG << "swap chain recreating: [" << extent.width << ", " << extent.height << "]";

    swapchain.create(extent, false);
    recreateCommandBuffers();
}

void Graphics::recreateCommandBuffers() {
    for (size_t i = 0; i < flightFences.size(); i++) {
        context.device.destroyFence(flightFences[i]);
        context.device.destroySemaphore(renderCompletes[i]);
        context.device.destroySemaphore(presentCompletes[i]);
    }

    presentCompletes.resize(swapchain.imageCount);
    renderCompletes.resize(swapchain.imageCount);
    flightFences.resize(swapchain.imageCount);

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < flightFences.size(); i++) {
        presentCompletes[i] = context.device.createSemaphore({});
        renderCompletes[i] = context.device.createSemaphore({});
        flightFences[i] = context.device.createFence({vk::FenceCreateFlagBits::eSignaled});
    }

    if (!commandBuffers.empty())
        context.device.freeCommandBuffers(context.getCommandPool(), commandBuffers);

    commandBuffers = context.allocateCommandBuffers(swapchain.imageCount);
}

void Graphics::recreateAttachmentsMap() {
    /*attachments.clear();

    for (const auto& renderStage : renderer->renderStages)
        attachments.insert(renderStage->descriptors.begin(), renderStage->descriptors.end());*/
}

void Graphics::recreatePass(RenderStage& renderStage) {
    auto extent = window->getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = window->getExtent();
        window->waitEvents();
    }

    context.graphicsQueue.waitIdle();

    if (renderStage.hasSwapchain() && (framebufferResized || swapchain.extent != extent))
        recreateSwapchain();

    renderStage.rebuild(swapchain);
    recreateAttachmentsMap(); // TODO: Maybe not recreate on a single change.
}

bool Graphics::startRenderpass(RenderStage& renderStage) {
    if (renderStage.isOutOfDate()) {
        recreatePass(renderStage);
        return false;
    }

    auto& commandBuffer = commandBuffers[swapchain.currentImage];

    commandBuffer.begin({ vk::CommandBufferUsageFlagBits::eSimultaneousUse });

    vk::Rect2D scissor{ vkx::util::rect2D(renderStage.renderArea.extent, renderStage.renderArea.offset) };
    vk::Viewport viewport{ vkx::util::viewport(renderStage.renderArea.extent) };

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);

    auto clearValues = renderStage.getClearValues();

    vk::RenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo.renderPass = *renderStage.getRenderpass();
    renderPassBeginInfo.framebuffer = renderStage.getActiveFramebuffer(swapchain.currentImage);
    renderPassBeginInfo.renderArea = scissor; // same as scissor
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();
    renderPassBeginInfo.pNext = nullptr;
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    return true;
}

void Graphics::endRenderpass(RenderStage& renderStage) {
    auto& commandBuffer = commandBuffers[swapchain.currentImage];

    commandBuffer.endRenderPass();

    if (!renderStage.hasSwapchain())
        return;

    commandBuffer.end();

    context.submit(commandBuffer, presentCompletes[currentFrame], vk::PipelineStageFlagBits::eColorAttachmentOutput, renderCompletes[currentFrame], flightFences[currentFrame]);

    auto presentResult = swapchain.queuePresent(renderCompletes[currentFrame]);
    if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR) {
        framebufferResized = true;
        //recreateSwapchain();
    } else if (presentResult != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swap chain image: " + vk::to_string(presentResult));
    }

    currentFrame = (currentFrame + 1) % swapchain.imageCount;
}

