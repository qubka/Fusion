#include "window.hpp"
#if !defined(ANDROID)

#include "fusion/events/window_events.hpp"
#include "fusion/events/application_events.hpp"

using namespace Fusion;

uint8_t Window::GLFWwindowCount{0};
std::vector<GLFWwindow*> Window::instances;

Window::Window(std::string title, const glm::uvec2& size, const glm::ivec2& position, bool fullscreen) :
    title{std::move(title)},
    width{static_cast<int>(size.x)},
    height{static_cast<int>(size.y)},
    aspect{static_cast<float>(size.x) / static_cast<float>(size.y)},
    minimize{width == 0 || height == 0},
    fullscreen{fullscreen}
{
    FE_ASSERT(width >= 0 && height >= 0 && "width or height cannot be negative");

    if (GLFWwindowCount == 0) {
        int success = glfwInit();
        if (!success) throw std::runtime_error("Could not initialize GLFW!");
        glfwSetErrorCallback(ErrorCallback);
        glfwSetMonitorCallback(MonitorCallback);
#if GLFW_VERSION_MINOR >= 2
        glfwSetJoystickCallback(JoystickCallback);
#endif
    }

    initWindow(position);

    instances.push_back(window);
}

Window::~Window() {
    instances.erase(std::remove(instances.begin(), instances.end(), window), instances.end());

    glfwDestroyWindow(window);
    window = nullptr;

    GLFWwindowCount--;
    if (GLFWwindowCount == 0) {
        glfwTerminate();
    }
}

void Window::initWindow(const glm::ivec2& position) {
    FE_LOG_INFO << "Creating window: " << title << " [" << width << " " << height << "]";

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    /*if (fullscreen) {
        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;
        aspect = static_cast<float>(width) / static_cast<float>(height);

        window = glfwCreateWindow(width, height, title.c_str(), monitor, nullptr);
    } else {
        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    }*/

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) throw std::runtime_error("Failed to create window!");
    GLFWwindowCount++;

    if (position != glm::ivec2{ 0, 0 }) {
        glfwSetWindowPos(window, position.x, position.y);
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetWindowPosCallback(window, PosCallback);
    glfwSetWindowSizeCallback(window, SizeCallback);
    glfwSetWindowCloseCallback(window, CloseCallback);
    glfwSetWindowRefreshCallback(window, RefreshCallback);
    glfwSetWindowFocusCallback(window, FocusCallback);
    glfwSetWindowIconifyCallback(window, IconifyCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    /* Setup inputs classes */
    //glfwSetMouseButtonCallback(window, MouseButtonCallback);
    //glfwSetCursorPosCallback(window, CursorPosCallback);
    //glfwSetCursorEnterCallback(window, CursorEnterCallback);
    //glfwSetScrollCallback(window, ScrollCallback);
    //glfwSetKeyCallback(window, KeyCallback);
    //glfwSetCharCallback(window, CharCallback);
#if GLFW_VERSION_MINOR >= 1
    glfwSetDropCallback(window, FileDropCallback);
#endif
#if GLFW_VERSION_MINOR >= 3
    glfwSetWindowMaximizeCallback(window, MaximizeCallback);
    glfwSetWindowContentScaleCallback(window, ContentScaleCallback);
#endif
}

#if defined(VULKAN_HPP)
std::vector<std::string> Window::getRequiredInstanceExtensions() {
    std::vector<std::string> result;
    uint32_t count = 0;
    const char** names = glfwGetRequiredInstanceExtensions(&count);
    if (names && count) {
        for (uint32_t i = 0; i < count; ++i) {
            result.emplace_back(names[i]);
        }
    }
    return result;
}

vk::SurfaceKHR Window::createWindowSurface(GLFWwindow* window, const vk::Instance& instance, const vk::AllocationCallbacks* pAllocator) {
    VkSurfaceKHR rawSurface;
    auto result = static_cast<vk::Result>(glfwCreateWindowSurface((VkInstance)instance, window, reinterpret_cast<const VkAllocationCallbacks*>(pAllocator), &rawSurface));
    return vk::createResultValue(result, rawSurface, "vk::CommandBuffer::begin");
}
#endif

glm::vec4 Window::getViewport() const {
#ifdef FE_VULKAN
    return { 0, 0, width, height };
#else // OPENGL
    return { 0, height, width, -height }; // vertical flip is required
#endif
}

void Window::PosCallback(GLFWwindow* handle, int x, int y) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.getEventQueue().submit(new WindowMovedEvent{{}, {x, y}});
}

void Window::SizeCallback(GLFWwindow* handle, int width, int height) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    /*window.width = width;
    window.height = height;
    window.aspect = static_cast<float>(width) / static_cast<float>(height);
    window.minimize = width == 0 || height == 0;*/

    window.getEventQueue().submit(new WindowSizeEvent{{}, width, height});
}

void Window::CloseCallback(GLFWwindow* handle) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.getEventQueue().submit(new WindowCloseEvent{});
}

void Window::RefreshCallback(GLFWwindow* handle) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.getEventQueue().submit(new WindowRefreshEvent{});
}

void Window::FocusCallback(GLFWwindow* handle, int focused) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    if (focused)
        window.getEventQueue().submit(new WindowFocusedEvent{});
    else
        window.getEventQueue().submit(new WindowUnfocusedEvent{});
}

void Window::IconifyCallback(GLFWwindow* handle, int iconified) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    if (iconified)
        window.getEventQueue().submit(new WindowDeiconifiedEvent{});
    else
        window.getEventQueue().submit(new WindowDeiconifiedEvent{});
}

void Window::FramebufferSizeCallback(GLFWwindow* handle, int width, int height) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.width = width;
    window.height = height;
    window.aspect = static_cast<float>(width) / static_cast<float>(height);
    window.minimize = width == 0 || height == 0;

    window.getEventQueue().submit(new WindowFramebufferSizeEvent{{}, width, height});
}

#if GLFW_VERSION_MINOR >= 1
void Window::FileDropCallback(GLFWwindow* handle, int count, const char** paths) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.getEventQueue().submit(new WindowFileDropCallback{{}, count, paths});
}
#endif

#if GLFW_VERSION_MINOR >= 2
void Window::JoystickCallback(int jid, int action) {
    for (auto* handle : instances) {
        auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

        switch (action) {
            case GLFW_CONNECTED:
                window.getEventQueue().submit(new JoystickConnectedEvent{{}, jid});
                break;
            case GLFW_DISCONNECTED:
                window.getEventQueue().submit(new JoystickDisconnectedEvent{{}, jid});
                break;
        }
    }
}
#endif

#if GLFW_VERSION_MINOR >= 3
void Window::MaximizeCallback(GLFWwindow* handle, int maximized) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    if (maximized)
        window.getEventQueue().submit(new WindowMaximizedEvent{});
    else
        window.getEventQueue().submit(new WindowUnmaximizedEvent{});
}

void Window::ContentScaleCallback(GLFWwindow* handle, float scaleX, float scaleY) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.getEventQueue().submit(new WindowContentScaleEvent{{},{scaleX, scaleY}});
}
#endif

void Window::MonitorCallback(GLFWmonitor* monitor, int action) {
    for (auto* handle : instances) {
        auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

        switch (action) {
            case GLFW_CONNECTED:
                window.getEventQueue().submit(new MonitorConnectedEvent{{}, monitor});
                break;
            case GLFW_DISCONNECTED:
                window.getEventQueue().submit(new MonitorDisconnectedEvent{{}, monitor});
                break;
        }
    }
}

void Window::ErrorCallback(int error, const char* description) {
    FE_LOG_ERROR << "[GLFW] Error (" << error << "): " << description;
}
#endif