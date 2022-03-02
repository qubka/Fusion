#include "renderer.hpp"
#include "fusion/core/application.hpp"

using namespace fe;

void Renderer::create() {
    commandPool = context.getCommandPool();

    offscreen.size = { 512, 512 };
    offscreen.create();

    recreateSwapChain();
    createUniformBuffers();
    createDescriptorSets();
    createCommandBuffers();
}

void Renderer::destroy() {
    device.freeCommandBuffers(commandPool, commandBuffers);

    offscreen.destroy();

    for (auto& buffer : uniformBuffers) {
        buffer.destroy();
    }
    uniformBuffers.clear();

    swapChain.destroy();

    descriptorLayoutCache.destroy();

    for (auto& allocator : dynamicAllocators) {
        allocator.destroy();
    }
    dynamicAllocators.clear();
    globalAllocator.destroy();
}

void Renderer::createUniformBuffers() {
    uniformBuffers.reserve(vkx::SwapChain::MAX_FRAMES_IN_FLIGHT);

    /* Global Ubo */
    for (int i = 0; i < vkx::SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers.push_back(context.createUniformBuffer(GlobalUbo{}));
    }
}

void Renderer::createDescriptorSets() {
    dynamicAllocators.reserve(vkx::SwapChain::MAX_FRAMES_IN_FLIGHT);
    globalDescriptorSets.resize(vkx::SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < vkx::SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        auto& descriptorAllocator = dynamicAllocators.emplace_back(context.device);
        vkx::DescriptorBuilder{descriptorLayoutCache, descriptorAllocator}
                .bindBuffer(0, &uniformBuffers[i].descriptor, vk::DescriptorType::eUniformBuffer,vk::ShaderStageFlagBits::eVertex)
                .build(globalDescriptorSets[i], globalDescriptorSetLayout);
    }

    /*vk::DescriptorSet textureSet;
    DescriptorBuilder(descriptorLayoutCache, globalAllocator)
            .bindImage(0, nullptr, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
            .build(textureSet, textureLayoutSet);*/
}

void Renderer::createCommandBuffers() {
    commandBuffers = context.allocateCommandBuffers(vkx::SwapChain::MAX_FRAMES_IN_FLIGHT);
}

void Renderer::recreateSwapChain() {
    auto& window = Application::Instance().getWindow();

    auto extent = window.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = window.getExtent();
        window.waitEvents();
    }

    device.waitIdle();

    LOG_DEBUG << "swap chain out of date/suboptimal/window resized - recreating";

    swapChain.create(extent, false);
}

uint32_t Renderer::beginFrame() {
    assert(!isFrameStarted && "cannot call beginFrame while already in progress");

    auto result = swapChain.acquireNextImage(currentImage);

#if !defined(__ANDROID__)
    // Recreate the swapchain if it's no longer compatible with the surface (OUT_OF_DATE) or no longer optimal for presentation (SUBOPTIMAL)
    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return UINT32_MAX;
    }
#endif

    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire next image");
    }

    vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

    commandBuffers[currentFrame].begin(beginInfo);
    if (offscreen.active) {
        offscreen.commandBuffers[currentFrame].begin(beginInfo);
    }

    isFrameStarted = true;

    return currentFrame;
}

void Renderer::beginRenderPass(uint32_t frameIndex) {
    assert(isFrameStarted && "cannot call beginRenderPass if frame is not in progress");
    assert(frameIndex == currentFrame && "cannot start render pass on command buffer from a different frame");

    const auto& extent = swapChain.extent;
    auto offset = vk::Offset2D{0, 0};

    clearValues[0].color = std::array<float, 4>{ color.x, color.y, color.z, 1 };
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    vk::Viewport viewport = vkx::util::viewport(extent);
    vk::Rect2D scissor = vkx::util::rect2D(extent, offset);

    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.renderPass = swapChain.renderPass;
    renderPassInfo.framebuffer = swapChain.framebuffers[currentImage];
    renderPassInfo.renderArea.offset = offset;
    renderPassInfo.renderArea.extent = extent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    auto& mainCmdBuffer = commandBuffers[currentFrame];
    mainCmdBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    mainCmdBuffer.setViewport(0, 1, &viewport);
    mainCmdBuffer.setScissor(0, 1, &scissor);

    if (offscreen.active) {
        vk::RenderPassBeginInfo offPassInfo;
        offPassInfo.renderPass = offscreen.renderPass;
        offPassInfo.framebuffer = offscreen.framebuffers[currentFrame].framebuffer;
        offPassInfo.renderArea.extent.width = offscreen.size.x;
        offPassInfo.renderArea.extent.width = offscreen.size.y;
        offPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        offPassInfo.pClearValues = clearValues.data();

        auto& offCmdBuffer = offscreen.commandBuffers[currentFrame];
        offCmdBuffer.beginRenderPass(offPassInfo, vk::SubpassContents::eInline);
        offCmdBuffer.setViewport(0, 1, &viewport);
        offCmdBuffer.setScissor(0, 1, &scissor);
    }
}

void Renderer::endRenderPass(uint32_t frameIndex) {
    assert(isFrameStarted && "cannot call endRenderPass if frame is not in progress");
    assert(frameIndex == currentFrame && "cannot end render pass on command buffer from a different frame");

    commandBuffers[currentFrame].endRenderPass();
    if (offscreen.active) {
        offscreen.commandBuffers[currentFrame].endRenderPass();
    }
}

void Renderer::endFrame(uint32_t frameIndex) {
    assert(isFrameStarted && "cannot call endFrame if frame is not in progress");
    assert(frameIndex == currentFrame && "cannot end command buffer from a different frame");

    std::vector<vk::CommandBuffer> buffers;
    buffers.reserve(offscreen.active ? 2 : 1);

    if (offscreen.active) {
        buffers.push_back(offscreen.commandBuffers[currentFrame]);
    }
    buffers.push_back(commandBuffers[currentFrame]);

    for (auto& buffer : buffers) {
        buffer.end();
    }

    auto result = swapChain.submitCommandBuffers(buffers, currentImage);

#if !defined(__ANDROID__)
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        recreateSwapChain();
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swap chain image");
    }
#else
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swap chain image");
    }
#endif

    isFrameStarted = false;
    currentFrame = (currentFrame + 1) % vkx::SwapChain::MAX_FRAMES_IN_FLIGHT;
}