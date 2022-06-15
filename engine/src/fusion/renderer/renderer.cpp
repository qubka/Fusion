#include "renderer.hpp"
#include "fusion/core/application.hpp"

#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>

using namespace fe;

void Renderer::create(const vk::Extent2D& size, bool overlay) {
    commandPool = context.getCommandPool();

    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    swapChain.create(size, false);

    createUniformBuffers();
    createDescriptorSets();
    createCommandBuffers();
    if (overlay) {
        createGui();
    }

    meshRenderer = new MeshRenderer(context, *this);
    gridRenderer = new GridRenderer(context, *this);
    skyRenderer = new SkyRenderer(context, *this);
}

void Renderer::destroy() {
    context.device.freeCommandBuffers(commandPool, commandBuffers);

    offscreen.destroy();

    gui.destroy();

    for (auto& buffer : uniformBuffers) {
        buffer.destroy();
    }

    delete meshRenderer;
    delete gridRenderer;
    delete skyRenderer;

    swapChain.destroy();

    descriptorLayoutCache.destroy();

    for (auto& allocator : dynamicAllocators) {
        allocator.destroy();
    }
    globalAllocator.destroy();
}

void Renderer::createUniformBuffers() {
    uniformBuffers.reserve(MAX_FRAMES_IN_FLIGHT);

    /* Global Ubo */
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers.push_back(context.createUniformBuffer(GlobalUbo{}));
    }
}

void Renderer::createDescriptorSets() {
    dynamicAllocators.reserve(MAX_FRAMES_IN_FLIGHT);
    globalDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        auto& descriptorAllocator = dynamicAllocators.emplace_back(context.device);
        vkx::DescriptorBuilder{descriptorLayoutCache, descriptorAllocator}
            // Binding 0 : Vertex shader uniform buffer
            .bindBuffer(0, &uniformBuffers[i].descriptor, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex)
            .build(globalDescriptorSets[i], globalDescriptorSetLayout);
    }

    /*vk::DescriptorSet textureSet;
    vkx::DescriptorBuilder{descriptorLayoutCache, globalAllocator}
        // Binding 1 : Fragment shader image sampler
        .bindImage(0, nullptr, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
        .build(textureSet, textureLayoutSet);*/
}

void Renderer::createCommandBuffers() {
    commandBuffers = context.allocateCommandBuffers(MAX_FRAMES_IN_FLIGHT);
}

void Renderer::createGui() {
    // Setup default overlay creation info
    vkx::UIOverlayCreateInfo overlayCreateInfo;
    overlayCreateInfo.renderPass = swapChain.renderPass;
    overlayCreateInfo.size = swapChain.extent;
    overlayCreateInfo.window = Application::Instance().getWindow();

    ImGui::SetCurrentContext(ImGui::CreateContext());

    gui.create(overlayCreateInfo);

    for (auto& shader : overlayCreateInfo.shaders) {
        context.device.destroyShaderModule(shader.module);
        shader.module = vk::ShaderModule{};
    }

    offscreen.size = vk::Extent2D{swapChain.extent.width / 2, swapChain.extent.height / 2};
    offscreen.create(gui);
}

void Renderer::recreateSwapChain() {
    auto window = Application::Instance().getWindow();

    auto extent = window->getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = window->getExtent();
        window->waitEvents();
    }

    //context.queue.waitIdle();
    context.device.waitIdle();

    LOG_DEBUG << "swap chain recreating: [" << extent.width << ", " << extent.height << "]";

    swapChain.create(extent, false);

    if (gui.active) {
        gui.resize(swapChain.extent);
    }
}

uint32_t Renderer::beginFrame() {
    assert(!frameStarted && "cannot call beginFrame while already in progress");

    auto result = swapChain.acquireNextImage(currentImage);

#if !defined(__ANDROID__)
    // Recreate the swapchain if it's no longer compatible with the surface (OUT_OF_DATE) or no longer optimal for presentation (SUBOPTIMAL)
    if (result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapChain();
        return UINT32_MAX;
    }
#endif

    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire next image: " + to_string(result));
    }

    vk::CommandBufferBeginInfo beginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit };

    commandBuffers[currentFrame].begin(beginInfo);
    if (offscreen.active) {
        offscreen.commandBuffers[currentFrame].begin(beginInfo);
    }

    clearValues[0].color = vkx::util::clearColor({ color, 1.0f });

    frameStarted = true;

    return currentFrame;
}

void Renderer::beginRenderPass(uint32_t frameIndex) {
    assert(frameStarted && "cannot call beginRenderPass if frame is not in progress");
    assert(frameIndex == currentFrame && "cannot start render pass on command buffer from a different frame");

    vk::Offset2D offset{};
    vk::RenderPassBeginInfo renderPassInfo;
    renderPassInfo.renderArea.offset = offset;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    {
        renderPassInfo.renderPass = swapChain.renderPass;
        renderPassInfo.framebuffer = swapChain.framebuffers[currentImage];

        auto& size = swapChain.extent;
        renderPassInfo.renderArea.extent = size;
        vk::Rect2D scissor{ vkx::util::rect2D(size, offset) };
        vk::Viewport viewport{ vkx::util::viewport(size) };

        auto& commandBuffer = commandBuffers[currentFrame];
        commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
        commandBuffer.setViewport(0, 1, &viewport);
        commandBuffer.setScissor(0, 1, &scissor);
    }

    if (offscreen.active) {
        renderPassInfo.renderPass = offscreen.renderPass;
        renderPassInfo.framebuffer = offscreen.framebuffers[currentFrame].framebuffer;

        auto& size = offscreen.size;
        renderPassInfo.renderArea.extent = size;
        vk::Rect2D scissor{ vkx::util::rect2D(size, offset) };
        vk::Viewport viewport{ vkx::util::viewport(size) };

        auto& commandBuffer = offscreen.commandBuffers[currentFrame];
        commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
        commandBuffer.setViewport(0, 1, &viewport);
        commandBuffer.setScissor(0, 1, &scissor);
    }
}

void Renderer::endRenderPass(uint32_t frameIndex) {
    assert(frameStarted && "cannot call endRenderPass if frame is not in progress");
    assert(frameIndex == currentFrame && "cannot end render pass on command buffer from a different frame");

    auto& commandBuffer = commandBuffers[currentFrame];

    if (gui.active) {
        gui.draw(commandBuffer);
    }

    commandBuffer.endRenderPass();

    if (offscreen.active) {
        offscreen.commandBuffers[currentFrame].endRenderPass();
    }
}

void Renderer::endFrame(uint32_t frameIndex) {
    assert(frameStarted && "cannot call endFrame if frame is not in progress");
    assert(frameIndex == currentFrame && "cannot end command buffer from a different frame");

    auto& commandBuffer = commandBuffers[currentFrame];
    commandBuffer.end();

    vk::Result result;
    if (offscreen.active) {
        auto& offBuffer = offscreen.commandBuffers[currentFrame];
        offBuffer.end();

        result = swapChain.submitCommandBuffers({ offBuffer, commandBuffer }, currentImage);
    } else {
        result = swapChain.submitCommandBuffers(commandBuffer, currentImage);
    }

#if !defined(__ANDROID__)
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        recreateSwapChain();
    } else if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swap chain image: " + to_string(result));
    }
#else
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swap chain image: " + to_string(result));
    }
#endif

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    frameStarted = false;
}

bool Renderer::beginGui(float dt) const {
    if (!gui.active)
        return false;

    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = dt;

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    return true;
}

void Renderer::endGui() {
    ImGui::Render();
    gui.update();
}