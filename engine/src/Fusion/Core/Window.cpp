#include "Window.hpp"

#include "Fusion/Events/WindowEvents.hpp"
#include "Fusion/Events/ApplicationEvents.hpp"

using namespace Fusion;

uint8_t Window::GLFWwindowCount{0};
std::vector<GLFWwindow*> Window::instances;

Window::Window(std::string title, int width, int height) :
    title{std::move(title)},
    width{width},
    height{height},
    aspect{static_cast<float>(width) / static_cast<float>(height)},
    minimize{width == 0 || height == 0}
{
    FE_ASSERT(width >= 0 && height >= 0 && "width or height cannot be negative");

    if (GLFWwindowCount == 0) {
        int success = glfwInit();
        FE_ASSERT(success && "could not initialize GLFW!");
        glfwSetErrorCallback(ErrorCallback);
        glfwSetMonitorCallback(MonitorCallback);
#if GLFW_VERSION_MINOR >= 2
        glfwSetJoystickCallback(JoystickCallback);
#endif
    }

    init();

    instances.push_back(window);
}

Window::~Window() {
    instances.erase(std::remove(instances.begin(), instances.end(), window), instances.end());

    glfwDestroyWindow(window);

    GLFWwindowCount--;
    if (GLFWwindowCount == 0) {
        glfwTerminate();
    }
}

void Window::init() {
    FE_LOG_INFO << "Creating window: " << title << " [" << width << " " << height << "]";

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
    FE_ASSERT(window && "failed to create window!");
    GLFWwindowCount++;

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

void Window::onUpdate() {
    eventQueue.free();
    glfwPollEvents();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::shouldClose(bool flag) const {
    glfwSetWindowShouldClose(window, flag);
}

glm::vec4 Window::getViewport() const {
#ifdef GLFW_INCLUDE_VULKAN
    return {0, 0, width, height};
#else // OPENGL
    return {0, height, width, -height}; // vertical flip is required
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