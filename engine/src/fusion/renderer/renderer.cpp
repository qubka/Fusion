#include "renderer.hpp"
#include "fusion/core/application.hpp"

using namespace fe;

void Renderer::create() {
    commandPool = context.getCommandPool();
    createRenderPass();
    recreateSwapChain();
    createUniformBuffers();
    createDescriptorSets();
    createCommandBuffers();
}

void Renderer::destroy() {
    device.freeCommandBuffers(commandPool, commandBuffers);

    for (auto& buffer : uniformBuffers) {
        buffer.destroy();
    }
    uniformBuffers.clear();

    swapChain.destroy();

    device.destroyRenderPass(renderPass);

    descriptorLayoutCache.destroy();

    for (auto& allocator : dynamicAllocators) {
        allocator.destroy();
    }
    dynamicAllocators.clear();
    globalAllocator.destroy();
}

void Renderer::createRenderPass() {
    std::vector<vk::AttachmentDescription> attachments;
    attachments.resize(2);

    // Color attachment
    attachments[0].format = colorformat;
    attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
    attachments[0].storeOp = vk::AttachmentStoreOp::eStore;
    attachments[0].initialLayout = vk::ImageLayout::eUndefined;
    attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

    // Depth attachment
    attachments[1].format = depthFormat;
    attachments[1].loadOp = vk::AttachmentLoadOp::eClear;
    attachments[1].storeOp = vk::AttachmentStoreOp::eDontCare;
    attachments[1].stencilLoadOp = vk::AttachmentLoadOp::eClear;
    attachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachments[1].initialLayout = vk::ImageLayout::eUndefined;
    attachments[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    // Only one depth attachment, so put it first in the references
    vk::AttachmentReference depthReference;
    depthReference.attachment = 1;
    depthReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    std::vector<vk::AttachmentReference> colorAttachmentReferences;
    {
        vk::AttachmentReference colorReference;
        colorReference.attachment = 0;
        colorReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
        colorAttachmentReferences.push_back(colorReference);
    }

    using vPSFB = vk::PipelineStageFlagBits;
    using vAFB = vk::AccessFlagBits;
    std::vector<vk::SubpassDependency> subpassDependencies{
            {
                    0, VK_SUBPASS_EXTERNAL,
                    vPSFB::eColorAttachmentOutput, vPSFB::eBottomOfPipe,
                    vAFB::eColorAttachmentRead | vAFB::eColorAttachmentWrite, vAFB::eMemoryRead,
                    vk::DependencyFlagBits::eByRegion
            },
            {
                    VK_SUBPASS_EXTERNAL, 0,
                    vPSFB::eBottomOfPipe, vPSFB::eColorAttachmentOutput,
                    vAFB::eMemoryRead, vAFB::eColorAttachmentRead | vAFB::eColorAttachmentWrite,
                    vk::DependencyFlagBits::eByRegion
            },
    };
    std::vector<vk::SubpassDescription> subpasses{
            {
                    {}, vk::PipelineBindPoint::eGraphics,
                    // Input attachment references
                    0, nullptr,
                    // Color / resolve attachment references
                    static_cast<uint32_t>(colorAttachmentReferences.size()), colorAttachmentReferences.data(), nullptr,
                    // Depth stecil attachment reference,
                    &depthReference,
                    // Preserve attachments
                    0, nullptr
            },
    };

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
    renderPassInfo.pSubpasses = subpasses.data();
    renderPassInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassInfo.pDependencies = subpassDependencies.data();
    renderPass = device.createRenderPass(renderPassInfo);
}

void Renderer::createUniformBuffers() {
    uniformBuffers.reserve(vkx::SwapChain::MAX_FRAMES_IN_FLIGHT);

    /* Global Ubo */
    for (int i = 0; i < vkx::SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers.push_back(context.createUniformBuffer(globalUbo));
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
    commandBuffers = device.allocateCommandBuffers({ commandPool, vk::CommandBufferLevel::ePrimary, vkx::SwapChain::MAX_FRAMES_IN_FLIGHT });
}

void Renderer::recreateSwapChain() {
    auto& window = Application::Instance().getWindow();

    auto extent = window.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = window.getExtent();
        window.waitEvents();
    }

    device.waitIdle();

    FE_LOG_DEBUG << "swap chain out of date/suboptimal/window resized - recreating";

    swapChain.create(extent, renderPass, false);
}

vk::CommandBuffer Renderer::beginFrame() {
    assert(!isFrameStarted && "cannot call beginFrame while already in progress");

    auto result = swapChain.acquireNextImage(currentImage);

#if !defined(__ANDROID__)
    // Recreate the swapchain if it's no longer compatible with the surface (OUT_OF_DATE) or no longer optimal for presentation (SUBOPTIMAL)
    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return nullptr;
    }
#endif

    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire next image");
    }

    const auto& commandBuffer = commandBuffers[currentFrame];
    vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

    commandBuffer.begin(beginInfo);

    isFrameStarted = true;

    return commandBuffer;
}

void Renderer::beginRenderPass(vk::CommandBuffer& commandBuffer) {
    assert(isFrameStarted && "cannot call beginRenderPass if frame is not in progress");

    const auto& extent = swapChain.extent;
    auto offset = vk::Offset2D{0, 0};

    clearValues[0].color = std::array<float, 4>{ color.x, color.y, color.z, 1 };
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChain.framebuffers[currentImage];
    renderPassInfo.renderArea.offset = offset;
    renderPassInfo.renderArea.extent = extent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport = vkx::util::viewport(extent);
    vk::Rect2D scissor = vkx::util::rect2D(extent, offset);

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);
}

void Renderer::endRenderPass(vk::CommandBuffer& commandBuffer) {
    assert(isFrameStarted && "cannot call endRenderPass if frame is not in progress");
    assert(commandBuffer == commandBuffers[currentFrame] && "cannot end render pass on command buffer from a different frame");

    commandBuffer.endRenderPass();
}

void Renderer::endFrame(vk::CommandBuffer& commandBuffer) {
    assert(isFrameStarted && "cannot call endFrame if frame is not in progress");
    assert(commandBuffer == commandBuffers[currentFrame] && "cannot end command buffer from a different frame");

    commandBuffer.end();

    auto result = swapChain.submitCommandBuffers(commandBuffer, currentImage);
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