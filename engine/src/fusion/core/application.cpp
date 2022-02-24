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

    renderer.destroy();

    ui.destroy();

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
        mainInit();
#endif
        mainLoop();

        // Once we exit the render loop, wait for everything to become idle before proceeding to the descructor.
        context.queue.waitIdle();
        context.device.waitIdle();
    } catch(const std::system_error& err) {
        std::cerr << err.what() << std::endl;
    }
}

void Application::mainInit() {
    setupWindow();
    initVulkan();
    setupRenderer();
    setupUi();
}

void Application::setupUi() {
    if (!settings.overlay) {
        return;
    }

    auto& swapChain = renderer.getSwapChain();

    struct vkx::ui::UIOverlayCreateInfo overlayCreateInfo;
    // Setup default overlay creation info
    overlayCreateInfo.copyQueue = queue;
    //overlayCreateInfo.framebuffers = swapChain.framebuffers;
    overlayCreateInfo.colorformat = swapChain.getColorFormat();
    overlayCreateInfo.depthformat = swapChain.getDepthFormat();
    overlayCreateInfo.size = size;

    ImGui::SetCurrentContext(ImGui::CreateContext());

    // Virtual function call for example to customize overlay creation
    //onPreSetupUIOverlay(overlayCreateInfo);
    ui.create(overlayCreateInfo);

    for (auto& shader : overlayCreateInfo.shaders) {
        context.device.destroyShaderModule(shader.module);
        shader.module = vk::ShaderModule{};
    }

    updateOverlay();
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
    auto surface = context.instance.createAndroidSurfaceKHR({ {}, android::androidApp->window });
#else
    auto surface = glfw::Window::createWindowSurface(reinterpret_cast<GLFWwindow*>(window->getNativeWindow()), context.instance);;
#endif

    context.createDevice(surface);
}

void Application::setupRenderer() {
    renderer.create();
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
                mainInit();
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

void Application::render() {
    if (renderer.beginFrame()) {
        auto commandBuffer = renderer.beginRender();

        for (auto* layer: layers) {
            layer->onRender();
        }

        drawUI(commandBuffer);

        renderer.endRender(commandBuffer);

        renderer.endFrame();
    }
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

void Application::drawUI(const vk::CommandBuffer& commandBuffer) {
    if (!settings.overlay) {
        return;
    }

    const vk::Viewport viewport = vkx::util::viewport(static_cast<float>(size.width), static_cast<float>(size.height), 0.0f, 1.0f);
    const vk::Rect2D scissor = vkx::util::rect2D(size.width, size.width, 0, 0);
    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);

    ui.draw(commandBuffer);
}

