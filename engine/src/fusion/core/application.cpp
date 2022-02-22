#include "application.hpp"
#include "layer.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

using namespace fe;

Application* Application::instance{nullptr};

// Avoid doing work in the ctor as it can't make use of overridden virtual functions
// Instead, use the `prepare` and `run` methods
Application::Application(std::string title, CommandLineArgs args) : title{std::move(title)}, commandLineArgs{args} {
    assert(!instance && "Application already exists!");
    instance = this;
#if defined(__ANDROID__)
    vkx::storage::setAssetManager(vkx::android::androidApp->activity->assetManager);
    vkx::android::androidApp->userData = this;
    vkx::android::androidApp->onInputEvent = ExampleBase::handle_input_event;
    vkx::android::androidApp->onAppCmd = ExampleBase::handle_app_cmd;
#endif
    //camera.setPerspective(60.0f, size.width / size.height, 0.1f, 256.0f);
}

Application::~Application() {
    instance = nullptr;

    context.queue.waitIdle();
    context.device.waitIdle();

    // Clean up Vulkan resources
    swapChain.destroy();
    if (descriptorPool) {
        device.destroyDescriptorPool(descriptorPool);
    }
    if (!commandBuffers.empty()) {
        device.freeCommandBuffers(cmdPool, commandBuffers);
        commandBuffers.clear();
    }
    device.destroyRenderPass(renderPass);
    for (auto& framebuffer : framebuffers) {
        device.destroyFramebuffer(framebuffer);
    }

    depthStencil.destroy();

    device.destroySemaphore(semaphores.acquireComplete);
    device.destroySemaphore(semaphores.renderComplete);
    device.destroySemaphore(semaphores.overlayComplete);

    ui.destroy();

    context.instance.destroySurfaceKHR(surface);
    context.destroy();

#if defined(__ANDROID__)
    // todo : android cleanup (if required)
#else
    delete window;
#endif
}

void Application::run() {
    try {
// Android initialization is handled in APP_CMD_INIT_WINDOW event
#if !defined(__ANDROID__)
        setupWindow();
        initVulkan();
        setupSwapchain();
        prepare();
#endif
        mainLoop();

        // Once we exit the render loop, wait for everything to become idle before proceeding to the descructor.
        context.queue.waitIdle();
        context.device.waitIdle();
    } catch(const std::system_error& err) {
        std::cerr << err.what() << std::endl;
    }
}

void Application::initVulkan() {
    // TODO make this less stupid
    context.setDeviceFeaturesPicker([this](const vk::PhysicalDevice& device, vk::PhysicalDeviceFeatures2& features){
        if (deviceFeatures.textureCompressionBC) {
            enabledFeatures.textureCompressionBC = VK_TRUE;
        } else if (context.deviceFeatures.textureCompressionASTC_LDR) {
            enabledFeatures.textureCompressionASTC_LDR = VK_TRUE;
        } else if (context.deviceFeatures.textureCompressionETC2) {
            enabledFeatures.textureCompressionETC2 = VK_TRUE;
        }
        if (deviceFeatures.samplerAnisotropy) {
            enabledFeatures.samplerAnisotropy = VK_TRUE;
        }
        //getEnabledFeatures();
    });

#if defined(__ANDROID__)
    context.requireExtensions({ VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME });
#else
    context.requireExtensions(glfw::Window::getRequiredInstanceExtensions());
#endif
    context.requireDeviceExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });
    context.createInstance(version);

#if defined(__ANDROID__)
    surface = context.instance.createAndroidSurfaceKHR({ {}, android::androidApp->window });
#else
    surface = glfw::Window::createWindowSurface(reinterpret_cast<GLFWwindow*>(window->getNativeWindow()), context.instance);;
#endif

    context.createDevice(surface);

    // Find a suitable depth format
    depthFormat = context.getSupportedDepthFormat();

    // Create synchronization objects

    // A semaphore used to synchronize image presentation
    // Ensures that the image is displayed before we start submitting new commands to the queu
    semaphores.acquireComplete = device.createSemaphore({});
    // A semaphore used to synchronize command submission
    // Ensures that the image is not presented until all commands have been sumbitted and executed
    semaphores.renderComplete = device.createSemaphore({});

    semaphores.overlayComplete = device.createSemaphore({});

    renderWaitSemaphores.push_back(semaphores.acquireComplete);
    renderWaitStages.push_back(vk::PipelineStageFlagBits::eBottomOfPipe);
    renderSignalSemaphores.push_back(semaphores.renderComplete);
}

void Application::setupSwapchain() {
    swapChain.setup(context.physicalDevice, context.device, context.queue, context.queueIndices.graphics);
    swapChain.setSurface(surface);
}

void Application::prepare() {
    cmdPool = context.getCommandPool();

    swapChain.create(size, enableVsync);
    setupDepthStencil();
    setupRenderPass();
    setupRenderPassBeginInfo();
    setupFrameBuffer();
    setupUi();

    onLoadAssets();
}

void Application::setupDepthStencil() {
    depthStencil.destroy();

    vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    vk::ImageCreateInfo depthStencilCreateInfo;
    depthStencilCreateInfo.imageType = vk::ImageType::e2D;
    depthStencilCreateInfo.extent = vk::Extent3D{ size.width, size.height, 1 };
    depthStencilCreateInfo.format = depthFormat;
    depthStencilCreateInfo.mipLevels = 1;
    depthStencilCreateInfo.arrayLayers = 1;
    depthStencilCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc;
    depthStencil = context.createImage(depthStencilCreateInfo);

    context.setImageLayout(depthStencil.image, aspect, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::ImageViewCreateInfo depthStencilView;
    depthStencilView.viewType = vk::ImageViewType::e2D;
    depthStencilView.format = depthFormat;
    depthStencilView.subresourceRange.aspectMask = aspect;
    depthStencilView.subresourceRange.levelCount = 1;
    depthStencilView.subresourceRange.layerCount = 1;
    depthStencilView.image = depthStencil.image;
    depthStencil.view = device.createImageView(depthStencilView);
}

void Application::setupRenderPass() {
    if (renderPass) {
        device.destroyRenderPass(renderPass);
    }

    std::array<vk::AttachmentDescription, 2> attachments;

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

void Application::setupRenderPassBeginInfo() {
    clearValues.clear();
    clearValues.push_back(vkx::util::clearColor(glm::vec4{0.1, 0.1, 0.1, 1.0}));
    clearValues.push_back(vk::ClearDepthStencilValue{ 1.0f, 0 });

    renderPassBeginInfo = vk::RenderPassBeginInfo();
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.renderArea.extent = size;
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();
}

void Application::setupFrameBuffer() {
    // Recreate the frame buffers
    if (!framebuffers.empty()) {
        for (auto& framebuffer : framebuffers) {
            device.destroyFramebuffer(framebuffer);
        }
        framebuffers.clear();
    }

    std::array<vk::ImageView, 2> attachments;

    // Depth/Stencil attachment is the same for all frame buffers
    attachments[1] = depthStencil.view;

    vk::FramebufferCreateInfo framebufferCreateInfo;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferCreateInfo.pAttachments = attachments.data();
    framebufferCreateInfo.width = size.width;
    framebufferCreateInfo.height = size.height;
    framebufferCreateInfo.layers = 1;

    // Create frame buffers for every swap chain image
    framebuffers = swapChain.createFramebuffers(framebufferCreateInfo);
}

void Application::setupUi() {
    //settings.overlay = settings.overlay && (!benchmark.active);
    if (!settings.overlay) {
        return;
    }

    struct vkx::ui::UIOverlayCreateInfo overlayCreateInfo;
    // Setup default overlay creation info
    overlayCreateInfo.copyQueue = queue;
    overlayCreateInfo.framebuffers = framebuffers;
    overlayCreateInfo.colorformat = swapChain.colorFormat;
    overlayCreateInfo.depthformat = depthFormat;
    overlayCreateInfo.size = size;

    ImGui::SetCurrentContext(ImGui::CreateContext());

    // Virtual function call for example to customize overlay creation
    onPreSetupUIOverlay(overlayCreateInfo);
    ui.create(overlayCreateInfo);

    for (auto& shader : overlayCreateInfo.shaders) {
        context.device.destroyShaderModule(shader.module);
        shader.module = vk::ShaderModule{};
    }

    updateOverlay();
    onPostSetupUIOverlay();
}

void Application::drawCurrentCommandBuffer() {
    vk::Fence fence = swapChain.getSubmitFence();
    {
        uint32_t fenceIndex = currentBuffer;
        context.dumpster.push_back([fenceIndex, this] { swapChain.clearSubmitFence(fenceIndex); });
    }

    // Command buffer(s) to be sumitted to the queue
    context.emptyDumpster(fence);
    {
        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(renderWaitSemaphores.size());
        submitInfo.pWaitSemaphores = renderWaitSemaphores.data();
        submitInfo.pWaitDstStageMask = renderWaitStages.data();

        submitInfo.signalSemaphoreCount = static_cast<uint32_t>(renderSignalSemaphores.size());
        submitInfo.pSignalSemaphores = renderSignalSemaphores.data();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffers.data() + currentBuffer;
        // Submit to queue
        context.queue.submit(submitInfo, fence);
    }

    context.recycle();
}

void Application::clearCommandBuffers() {
    if (!commandBuffers.empty()) {
        context.trashCommandBuffers(cmdPool, commandBuffers);
        // FIXME find a better way to ensure that the draw and text buffers are no longer in use before
        // executing them within this command buffer.
        context.queue.waitIdle();
        context.device.waitIdle();
        context.recycle();
    }
}

void Application::allocateCommandBuffers() {
    clearCommandBuffers();
    // Create one command buffer per image in the swap chain

    // Command buffers store a reference to the
    // frame buffer inside their render pass info
    // so for static usage without having to rebuild
    // them each frame, we use one per frame buffer
    commandBuffers = device.allocateCommandBuffers({ cmdPool, vk::CommandBufferLevel::ePrimary, swapChain.imageCount });
}

void Application::buildCommandBuffers() {
    // Destroy and recreate command buffers if already present
    allocateCommandBuffers();

    vk::CommandBufferBeginInfo cmdBufInfo{ vk::CommandBufferUsageFlagBits::eSimultaneousUse };
    for (size_t i = 0; i < swapChain.imageCount; ++i) {
        const auto& cmdBuffer = commandBuffers[i];
        cmdBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
        cmdBuffer.begin(cmdBufInfo);
        onUpdateCommandBufferPreDraw(cmdBuffer);
        // Let child classes execute operations outside the renderpass, like buffer barriers or query pool operations
        renderPassBeginInfo.framebuffer = framebuffers[i];
        cmdBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
        onUpdateDrawCommandBuffer(cmdBuffer);
        cmdBuffer.endRenderPass();
        onUpdateCommandBufferPostDraw(cmdBuffer);
        cmdBuffer.end();
    }
}

void Application::addRenderWaitSemaphore(const vk::Semaphore& semaphore, const vk::PipelineStageFlags& waitStages) {
    renderWaitSemaphores.push_back(semaphore);
    renderWaitStages.push_back(waitStages);
}

void Application::mainLoop() {
    auto tStart = std::chrono::high_resolution_clock::now();

    window->runLoop([&]{
        auto tEnd = std::chrono::high_resolution_clock::now();
        auto tDiff = std::chrono::duration<float, std::milli>(tEnd - tStart).count();
        auto tDiffSeconds = tDiff / 1000.0f;
        tStart = tEnd;

        update(tDiffSeconds);

        if (!window->isMinimized()) {
            render();
        }
    });
}

std::string Application::getWindowTitle() {
    return title + " - " + std::string(context.deviceProperties.deviceName) + " - " + std::to_string(frameCounter) + " fps";
}

void Application::prepareFrame() {
    // Acquire the next image from the swap chaing
    auto result = swapChain.acquireNextImage(semaphores.acquireComplete);

#if !defined(__ANDROID__)
    // Recreate the swapchain if it's no longer compatible with the surface (OUT_OF_DATE) or no longer optimal for presentation (SUBOPTIMAL)
    if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR) {
        recreateSwapchain(window->getSize());
        result = swapChain.acquireNextImage(semaphores.acquireComplete);
    }
#endif

    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire next image");
    }

    currentBuffer = swapChain.currentImage;
}

void Application::submitFrame() {
    bool submitOverlay = settings.overlay && ui.visible && (ui.cmdBuffers.size() > currentBuffer);
    if (submitOverlay) {
        vk::SubmitInfo submitInfo;
        // Wait for color attachment output to finish before rendering the text overlay
        vk::PipelineStageFlags stageFlags = vk::PipelineStageFlagBits::eBottomOfPipe;
        submitInfo.pWaitDstStageMask = &stageFlags;
        // Wait for render complete semaphore
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &semaphores.renderComplete;
        // Signal ready with UI overlay complete semaphpre
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &semaphores.overlayComplete;

        // Submit current UI overlay command buffer
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &ui.cmdBuffers[currentBuffer];
        queue.submit({ submitInfo }, {});
    }
    swapChain.queuePresent(submitOverlay ? semaphores.overlayComplete : semaphores.renderComplete);
}

void Application::draw() {
    // Get next image in the swap chain (back/front buffer)
    prepareFrame();
    // Execute the compiled command buffer for the current swap chain image
    drawCurrentCommandBuffer();
    // Push the rendered frame to the surface
    submitFrame();
}

void Application::render() {
    draw();
}

void Application::update(float deltaTime) {
    ++frameNumber;
    frameTimer = deltaTime;
    ++frameCounter;

    fpsTimer += frameTimer;
    if (fpsTimer > 1.0f) {
#if !defined(__ANDROID__)
        reinterpret_cast<glfw::Window*>(window)->setTitle(getWindowTitle());
#endif
        lastFPS = frameCounter;
        fpsTimer = 0.0f;
        frameCounter = 0;
    }

    for (auto* layer: layers) {
        layer->onUpdate();
    }

    updateOverlay();
}

void Application::recreateSwapchain(const glm::uvec2& newSize) {
    queue.waitIdle();
    device.waitIdle();

    // Recreate swap chain
    size.width = newSize.x;
    size.height = newSize.y;
    swapChain.create(size, enableVsync);

    setupDepthStencil();
    setupFrameBuffer();
    setupRenderPassBeginInfo();

    if (settings.overlay) {
        ui.resize(size, framebuffers);
    }

    // Notify derived class
    onWindowResized();

    // Command buffers need to be recreated as they may store
    // references to the recreated frame buffer
    clearCommandBuffers();
    allocateCommandBuffers();
    buildCommandBuffers();

    onViewChanged();
}

void Application::updateOverlay() {
    if (!settings.overlay) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2{static_cast<float>(size.width), static_cast<float>(size.height)};
    io.DeltaTime = frameTimer;

    auto& mouseInput = window->getMouseInput();
    auto& mousePos = mouseInput.mousePosition();
    io.AddMousePosEvent(mousePos.x, mousePos.y);
    io.AddMouseButtonEvent(0, mouseInput.getMouseButton(Mouse::ButtonLeft));
    io.AddMouseButtonEvent(1,mouseInput.getMouseButton(Mouse::ButtonRight));
    io.AddMouseButtonEvent(3, mouseInput.getMouseButton(Mouse::ButtonMiddle));
    auto& mouseWheel = mouseInput.mouseScroll();
    io.AddMouseWheelEvent(mouseWheel.x, mouseWheel.y);

    ImGui::NewFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::BeginFrame();

    for (auto* layer: layers) {
        layer->onImGui();
    }

    ImGui::Render();
    ui.update();
}

#if defined(__ANDROID__)
int32_t Application::handle_input_event(android_app* app, AInputEvent* event) {
    auto& app = *reinterpret_cast<Application*>(app->userData);
    return app.window.onInput(event);
}

void Application::handle_app_cmd(android_app* app, int32_t cmd) {
    auto& app = *reinterpret_cast<Application*>(app->userData);
    app.onAppCmd(cmd);
}

void Application::onAppCmd(int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (vkx::android::androidApp->window != nullptr) {
                setupWindow();
                initVulkan();
                setupSwapchain();
                prepare();
            }
            break;
        case APP_CMD_LOST_FOCUS:
            window.setMinimized(false);
            break;
        case APP_CMD_GAINED_FOCUS:
            window.setMinimized(true)
            break;
        default:
            break;
    }
}

void Application::setupWindow() {
    window = new Window{};
    size.width = window.getWidth();
    size.height = window.getHeight();
    camera.updateAspectRatio(size);
}
#else

void Application::setupWindow() {
    bool fullscreen = false;

#ifdef _WIN32
    // Check command line arguments
    for (int32_t i = 0; i < __argc; i++) {
        if (__argv[i] == std::string("-fullscreen")) {
            fullscreen = true;
        }
    }
#endif

    window = new glfw::Window{title, {size.width, size.height}};
}
#endif

