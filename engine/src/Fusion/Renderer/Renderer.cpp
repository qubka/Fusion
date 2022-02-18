#include "Renderer.hpp"
#include "SwapChain.hpp"
#include "Offscreen.hpp"
#include "AllocatedBuffer.hpp"
#include "Descriptors.hpp"

#include "Fusion/Core/Window.hpp"

using namespace Fusion;

Renderer::Renderer(Vulkan& vulkan) : vulkan{vulkan}, globalAllocator{vulkan}, descriptorLayoutCache{vulkan} {
    offscreen = std::make_unique<Offscreen>(vulkan, vk::Extent2D{1280, 720});

    recreateSwapChain();
    createUniformBuffers();
    createDescriptorSets();
    createCommandBuffers();
}

Renderer::~Renderer() {
    vulkan.getDevice().freeCommandBuffers(vulkan.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    vulkan.getDevice().freeCommandBuffers(vulkan.getCommandPool(), static_cast<uint32_t>(offscreenBuffer.size()), offscreenBuffer.data());
}

void Renderer::createUniformBuffers() {
    uniformBuffers.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);

    /* Global Ubo */
    for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers.emplace_back(
                vulkan,
                sizeof(GlobalUbo),
                1,
                vk::BufferUsageFlagBits::eUniformBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        ).map();
    }
}

void Renderer::createDescriptorSets() {
    dynamicAllocators.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);
    globalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        auto& descriptorAllocator = dynamicAllocators.emplace_back(vulkan);
        auto bufferInfo = uniformBuffers[i].descriptorInfo();

        DescriptorBuilder{descriptorLayoutCache, descriptorAllocator}
                .bindBuffer(0, &bufferInfo, vk::DescriptorType::eUniformBuffer,vk::ShaderStageFlagBits::eVertex)
                .build(globalDescriptorSets[i], globalDescriptorSetLayout);
    }

    /*vk::DescriptorSet textureSet;
    DescriptorBuilder(descriptorLayoutCache, globalAllocator)
            .bindImage(0, nullptr, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
            .build(textureSet, textureLayoutSet);*/
}

void Renderer::createCommandBuffers() {
    {
        commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = vulkan.getCommandPool();
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = SwapChain::MAX_FRAMES_IN_FLIGHT;

        auto result = vulkan.getDevice().allocateCommandBuffers(&allocInfo, commandBuffers.data());
        FE_ASSERT(result == vk::Result::eSuccess && "failed to allocate descriptor command buffers!");
    }

    {
        offscreenBuffer.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = vulkan.getCommandPool();
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = SwapChain::MAX_FRAMES_IN_FLIGHT;

        auto result = vulkan.getDevice().allocateCommandBuffers(&allocInfo, offscreenBuffer.data());
        FE_ASSERT(result == vk::Result::eSuccess && "failed to allocate descriptor command buffers!");
    }

}

void Renderer::recreateSwapChain() {
    auto& window = vulkan.getWindow();

    auto extent = vk::Extent2D(window.getWidth(), window.getHeight());
    while (extent.width == 0 || extent.height == 0) {
        extent = vk::Extent2D(window.getWidth(), window.getHeight());
        glfwWaitEvents();
    }

    auto result = vulkan.getDevice().waitIdle();
    FE_ASSERT(result == vk::Result::eSuccess && "failed to wait on the device!");

    if (swapChain == nullptr) {
        swapChain = std::make_unique<SwapChain>(vulkan, extent);
    } else {
        std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain);
        swapChain = std::make_unique<SwapChain>(vulkan, extent, oldSwapChain);

        bool compatible = oldSwapChain->compareSwapFormats(*swapChain);
        FE_ASSERT(compatible && "swap chain image(or depth) format has changed");
    }
}

bool Renderer::beginFrame() {
    FE_ASSERT(!isFrameStarted && "cannot call beginFrame while already in progress");

    auto result = swapChain->acquireNextImage(currentImage);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return false;
    }

    FE_ASSERT((result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR) && "failed to acquire swap chain image");

    isFrameStarted = true;

    return true;
}

vk::CommandBuffer Renderer::beginSwapChainRenderPass() {
    FE_ASSERT(isFrameStarted && "cannot call beginSwapChainRenderPass if frame is not in progress");
    //FE_ASSERT(commandBuffer == commandBuffers[currentFrame] && "cannot begin render pass on command buffer from a different frame");

    const auto& extent = swapChain->getSwapChainExtent();
    auto offset = vk::Offset2D{0, 0};

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.renderPass = swapChain->getRenderPass();
    renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImage);
    renderPassInfo.renderArea.offset = offset;
    renderPassInfo.renderArea.extent = extent;

    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].color = std::array<float, 4>{ color.x, color.y, color.z, 1 };
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    const auto& commandBuffer = commandBuffers[currentFrame];
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    auto result = commandBuffer.begin(&beginInfo);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to begin recording command buffer!");

    commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0;
    viewport.maxDepth = 1;

    vk::Rect2D scissor{offset, extent};

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);

    return commandBuffer;
}

void Renderer::endSwapChainRenderPass(vk::CommandBuffer& commandBuffer) {
    FE_ASSERT(isFrameStarted && "cannot call endSwapChainRenderPass if frame is not in progress");
    //FE_ASSERT(commandBuffer == commandBuffers[currentFrame] && "cannot end render pass on command buffer from a different frame");

    commandBuffer.endRenderPass();

    auto result = commandBuffer.end();
    FE_ASSERT(result == vk::Result::eSuccess && "failed to record command buffer!");
}

vk::CommandBuffer Renderer::beginOffscreenRenderPass() {
    FE_ASSERT(isFrameStarted && "cannot call beginOffscreenRenderPass if frame is not in progress");
    //FE_ASSERT(commandBuffer == commandBuffers[currentFrame] && "cannot begin render pass on command buffer from a different frame");

    const auto& extent = offscreen->getExtent();
    auto offset = vk::Offset2D{0, 0};

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.renderPass = offscreen->getRenderPass();
    renderPassInfo.framebuffer = offscreen->getFrameBuffer();
    renderPassInfo.renderArea.offset = offset;
    renderPassInfo.renderArea.extent = extent;

    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].color = std::array<float, 4>{ color.x, color.y, color.z, 1 };
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    const auto& commandBuffer = offscreenBuffer[currentFrame];
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    auto result = commandBuffer.begin(&beginInfo);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to begin recording command buffer!");

    commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0;
    viewport.maxDepth = 1;

    vk::Rect2D scissor{offset, extent};

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);

    return commandBuffer;
}

void Renderer::endOffscreenRenderPass(vk::CommandBuffer& commandBuffer) {
    FE_ASSERT(isFrameStarted && "cannot call endOffscreenRenderPass if frame is not in progress");
    //FE_ASSERT(commandBuffer == commandBuffers[currentFrame] && "cannot end render pass on command buffer from a different frame");

    commandBuffer.endRenderPass();

    auto result = commandBuffer.end();
    FE_ASSERT(result == vk::Result::eSuccess && "failed to record command buffer!");
}

void Renderer::endFrame() {
    FE_ASSERT(isFrameStarted && "cannot call endFrame if frame is not in progress");
    //FE_ASSERT(commandBuffer == commandBuffers[currentFrame] && "cannot end command buffer from a different frame");

    auto result = swapChain->submitCommandBuffers({commandBuffers[currentFrame], offscreenBuffer[currentFrame]}, currentImage);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        FE_LOG_DEBUG << "swap chain out of date/suboptimal/window resized - recreating";
        recreateSwapChain();
    } else {
        FE_ASSERT(result == vk::Result::eSuccess && "failed to present swap chain image");
    }

    isFrameStarted = false;
    currentFrame = (currentFrame + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}
