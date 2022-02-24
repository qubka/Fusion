#include "application.hpp"
#include "layer.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

using namespace fe;

Application* Application::instance{nullptr};

// Avoid doing work in the ctor as it can't make use of overridden virtual functions
// Instead, use the `prepare` and `run` methods
Application::Application(std::string title, CommandLineArgs args) : title{std::move(title)} {
    assert(!instance && "Application already exists!");
    instance = this;
#if defined(__ANDROID__)
    vkx::storage::setAssetManager(vkx::android::androidApp->activity->assetManager);
    vkx::android::androidApp->userData = this;
    vkx::android::androidApp->onInputEvent = ExampleBase::handle_input_event;
    vkx::android::androidApp->onAppCmd = ExampleBase::handle_app_cmd;
#endif

#if !defined(VK_USE_PLATFORM_ANDROID_KHR)
    // Check for a valid asset path
    struct stat info;
    if (stat(getAssetPath().c_str(), &info) != 0) {
#if defined(_WIN32)
        std::string msg = "Could not locate asset path in \"" + getAssetPath() + "\" !";
		MessageBox(NULL, msg.c_str(), "Fatal error", MB_OK | MB_ICONERROR);
#else
        FE_LOG_ERROR << "Error: Could not find asset path in " << getAssetPath();
#endif
        exit(-1);
    }
#endif

    //settings.validation = enableValidation;

    // Command line arguments
    commandLineParser.parse(args);
    if (commandLineParser.isSet("help")) {
#if defined(_WIN32)
        setupConsole("Fusion Engine");
#endif
        commandLineParser.printHelp();
        std::cin.get();
        exit(0);
    }
    if (commandLineParser.isSet("validation")) {
        settings.validation = true;
    }
    if (commandLineParser.isSet("vsync")) {
        settings.vsync = true;
    }
    if (commandLineParser.isSet("height")) {
        size.height = commandLineParser.getValueAsInt("height", size.height);
    }
    if (commandLineParser.isSet("width")) {
        size.width = commandLineParser.getValueAsInt("width", size.width);
    }
    if (commandLineParser.isSet("fullscreen")) {
        settings.fullscreen = true;
    }
    if (commandLineParser.isSet("shaders")) {
        std::string value = commandLineParser.getValueAsString("shaders", "glsl");
        if ((value != "glsl") && (value != "hlsl")) {
            FE_LOG_ERROR << "Shader type must be one of 'glsl' or 'hlsl'";
        }/* else {
            shaderDir = value;
        }*/
    }
    if (commandLineParser.isSet("benchmark")) {
        benchmark.active = true;
        //vkx::util::errorModeSilent = true;
    }
    if (commandLineParser.isSet("benchmarkwarmup")) {
        benchmark.warmup = commandLineParser.getValueAsInt("benchmarkwarmup", benchmark.warmup);
    }
    if (commandLineParser.isSet("benchmarkruntime")) {
        benchmark.duration = commandLineParser.getValueAsInt("benchmarkruntime", benchmark.duration);
    }
    if (commandLineParser.isSet("benchmarkresultfile")) {
        benchmark.filename = commandLineParser.getValueAsString("benchmarkresultfile", benchmark.filename);
    }
    if (commandLineParser.isSet("benchmarkresultframes")) {
        benchmark.outputFrameTimes = true;
    }
    if (commandLineParser.isSet("benchmarkframes")) {
        benchmark.outputFrames = commandLineParser.getValueAsInt("benchmarkframes", benchmark.outputFrames);
    }

#if defined(_WIN32)
    // Enable console if validation is active, debug message callback will output to it
	if (settings.validation) {
		setupConsole("Fusion Engine");
	}
#endif
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
        FE_LOG_ERROR << err.what();
    }
}

void Application::mainInit() {
    setupWindow();
    initVulkan();
    setupRenderer();
    setupUi();
}

void Application::setupUi() {
    settings.overlay = settings.overlay && (!benchmark.active);
    if (!settings.overlay) {
        return;
    }

    struct vkx::ui::UIOverlayCreateInfo overlayCreateInfo;
    // Setup default overlay creation info
    overlayCreateInfo.renderPass = renderer.getRenderPass();
    //overlayCreateInfo.size = size;

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
    if (benchmark.active) {
        benchmark.run([=] { render(); }, physicalDevice.getProperties());
        device.waitIdle();
        if (!benchmark.filename.empty()) {
            benchmark.saveResults();
        }
        return;
    }

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

#if defined(_WIN32)
// Win32 : Sets up a console window and redirects standard output to it
void Application::setupConsole(std::string title)
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE *stream;
	freopen_s(&stream, "CONIN$", "r", stdin);
	freopen_s(&stream, "CONOUT$", "w+", stdout);
	freopen_s(&stream, "CONOUT$", "w+", stderr);
	SetConsoleTitle(TEXT(title.c_str()));
}
#endif

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
            window.setFocus(false);
            break;
        case APP_CMD_GAINED_FOCUS:
            window.setFocus(true)
            break;
        default:
            break;
    }
}

void Application::setupWindow() {
    window = new android::Window{};
}
#else

void Application::setupWindow() {
    if (settings.fullscreen) {
        window = new glfw::Window{title};
    } else {
        window = new glfw::Window{title, { size.width, size.height }};
    }
}
#endif

void Application::render() {
    if (auto cmdBuffer = renderer.beginFrame()) {
        renderer.beginRenderPass(cmdBuffer);

        for (auto* layer: layers) {
            layer->onRender();
        }

        if (settings.overlay) {
            ui.draw(cmdBuffer);
        }

        renderer.endRenderPass(cmdBuffer);

        renderer.endFrame(cmdBuffer);
    }
}

void Application::updateOverlay() {
    if (!settings.overlay) {
        return;
    }

    const auto& extent = window->getExtent();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2{static_cast<float>(extent.width), static_cast<float>(extent.height)};
    io.DeltaTime = frameTimer;

    auto& mouseInput = window->getMouseInput();
    auto& mousePos = mouseInput.mousePosition();
    io.AddMousePosEvent(mousePos.x, mousePos.y);
    io.AddMouseButtonEvent(0, mouseInput.getMouseButton(Mouse::Button0));
    io.AddMouseButtonEvent(1,mouseInput.getMouseButton(Mouse::Button1));
    io.AddMouseButtonEvent(2, mouseInput.getMouseButton(Mouse::Button2));
    io.AddMouseButtonEvent(3, mouseInput.getMouseButton(Mouse::Button3));
    io.AddMouseButtonEvent(4, mouseInput.getMouseButton(Mouse::Button4));

    /*window->getEventQueue().next<>()

    io.AddFocusEvent();
    io.AddInputCharacter();
    io.AddKeyEvent();*/

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

