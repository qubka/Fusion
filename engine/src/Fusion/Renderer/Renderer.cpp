#include "Renderer.hpp"
#include "SwapChain.hpp"
#include "AllocatedBuffer.hpp"
#include "Descriptors.hpp"

#include "Fusion/Core/Window.hpp"

using namespace Fusion;

Renderer::Renderer(Vulkan& vulkan) : vulkan{vulkan} {
    recreateSwapChain();
    createUniformBuffers();
    createDescriptorSets();
    createCommandBuffers();
}

Renderer::~Renderer() {
    vulkan.getDevice().freeCommandBuffers(vulkan.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
}

void Renderer::createCommandBuffers() {
    commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = vulkan.getCommandPool();
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = SwapChain::MAX_FRAMES_IN_FLIGHT;

    auto result = vulkan.getDevice().allocateCommandBuffers(&allocInfo, commandBuffers.data());
    FE_ASSERT(result == vk::Result::eSuccess && "failed to allocate descriptor command buffers!");
}

void Renderer::createUniformBuffers() {
    uniformBuffers.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        auto buffer = std::make_unique<AllocatedBuffer>(
                vulkan,
                sizeof(UniformBufferObject),
                1,
                vk::BufferUsageFlagBits::eUniformBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        buffer->map();
        uniformBuffers.push_back(std::move(buffer));
    }
}

void Renderer::createDescriptorSets() {
    globalPool = DescriptorPool::Builder(vulkan)
        .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(vk::DescriptorType::eUniformBuffer, SwapChain::MAX_FRAMES_IN_FLIGHT)
        .build();

    globalLayout = DescriptorLayout::Builder(vulkan)
        .addBinding(0, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex)
        .build();

    globalDescriptorSets.reserve(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorSet descriptorSet;
        auto bufferInfo = uniformBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalLayout, *globalPool)
            .writeBuffer(0, bufferInfo)
            .build(descriptorSet);
        globalDescriptorSets.push_back(descriptorSet);
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

vk::CommandBuffer Renderer::beginFrame() {
    FE_ASSERT(!isFrameStarted && "cannot call beginFrame while already in progress");

    auto result = swapChain->acquireNextImage(currentImage);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return nullptr;
    }

    FE_ASSERT((result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR) && "failed to acquire swap chain image");

    isFrameStarted = true;

    const auto& commandBuffer = commandBuffers[currentFrame];
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    result = commandBuffer.begin(&beginInfo);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to begin recording command buffer!");

    return commandBuffer;
}

void Renderer::beginSwapChainRenderPass(vk::CommandBuffer& commandBuffer) {
    FE_ASSERT(isFrameStarted && "cannot call beginSwapChainRenderPass if frame is not in progress");
    FE_ASSERT(commandBuffer == commandBuffers[currentFrame] && "cannot begin render pass on command buffer from a different frame");

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
}

void Renderer::endSwapChainRenderPass(vk::CommandBuffer& commandBuffer) {
    FE_ASSERT(isFrameStarted && "cannot call endSwapChainRenderPass if frame is not in progress");
    FE_ASSERT(commandBuffer == commandBuffers[currentFrame] && "cannot end render pass on command buffer from a different frame");

    commandBuffer.endRenderPass();
}

void Renderer::endFrame(vk::CommandBuffer& commandBuffer) {
    FE_ASSERT(isFrameStarted && "cannot call endFrame if frame is not in progress");
    FE_ASSERT(commandBuffer == commandBuffers[currentFrame] && "cannot end command buffer from a different frame");

    auto result = commandBuffer.end();
    FE_ASSERT(result == vk::Result::eSuccess && "failed to record command buffer!");

    result = swapChain->submitCommandBuffers(commandBuffer, currentImage);
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        FE_LOG_DEBUG << "swap chain out of date/suboptimal/window resized - recreating";
        recreateSwapChain();
    } else {
        FE_ASSERT(result == vk::Result::eSuccess && "failed to present swap chain image");
    }

    isFrameStarted = false;
    currentFrame = (currentFrame + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}

const vk::DescriptorSetLayout& Renderer::getGlobalLayoutSet() const {
    return globalLayout->getDescriptorSetLayout();
}
