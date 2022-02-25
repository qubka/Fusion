#include "window.hpp"
#if !defined(ANDROID)

using namespace glfw;

std::vector<GLFWwindow*> Window::instances;

Window::Window(std::string title, const glm::ivec2& size, const glm::ivec2& position)
    : fe::Window{size.x, size.y}
    , title{std::move(title)}
    , position{position}
{
    initGLFW();
    initWindow(false);

    instances.push_back(window);
}

Window::Window(std::string title)
    : fe::Window{1, 1}  // will be fullscreen
    , title{std::move(title)}
    , position{0, 0}
{
    initGLFW();
    initWindow(true);

    instances.push_back(window);
}

Window::~Window() {
    instances.erase(std::remove(instances.begin(), instances.end(), window), instances.end());

    glfwDestroyWindow(window);
    window = nullptr;

    if (instances.empty()) {
        glfwTerminate();
    }
}

void Window::initGLFW() {
    if (instances.empty()) {
        int success = glfwInit();
        if (!success) throw std::runtime_error("Could not initialize GLFW!");
        glfwSetErrorCallback(ErrorCallback);
        glfwSetMonitorCallback(MonitorCallback);
#if GLFW_VERSION_MINOR >= 2
        glfwSetJoystickCallback(JoystickCallback);
#endif
    }
}

void Window::initWindow(bool fullscreen) {
    FE_LOG_INFO << "Creating window: " << title << " [" << width << " " << height << "]";

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    if (fullscreen) {
        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;

        window = glfwCreateWindow(width, height, title.c_str(), monitor, nullptr);
    } else {
        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    }

    if (!window) throw std::runtime_error("Failed to create window!");

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
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetCursorEnterCallback(window, CursorEnterCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
#if GLFW_VERSION_MINOR >= 1
    glfwSetDropCallback(window, FileDropCallback);
#endif
#if GLFW_VERSION_MINOR >= 3
    glfwSetWindowMaximizeCallback(window, MaximizeCallback);
    glfwSetWindowContentScaleCallback(window, ContentScaleCallback);
#endif

    StartEvent();
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
    auto result = static_cast<vk::Result>(glfwCreateWindowSurface(instance, window, reinterpret_cast<const VkAllocationCallbacks*>(pAllocator), &rawSurface));
    return vk::createResultValue(result, rawSurface, "glfwCreateWindowSurface");
}
#endif

void Window::PosCallback(GLFWwindow* handle, int x, int y) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    glm::ivec2 pos {x, y};

    window.position = pos;

    window.PositionChangeEvent(pos);
}

void Window::SizeCallback(GLFWwindow* handle, int width, int height) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    glm::ivec2 size {width, height};

    window.SizeChangeEvent(size);
}

void Window::CloseCallback(GLFWwindow* handle) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.CloseEvent();
}

void Window::RefreshCallback(GLFWwindow* handle) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.RefreshEvent();
}

void Window::FocusCallback(GLFWwindow* handle, int focused) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.FocusEvent(focused);
}

void Window::IconifyCallback(GLFWwindow* handle, int iconified) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.IconifyEvent(iconified);
}

void Window::FramebufferSizeCallback(GLFWwindow* handle, int width, int height) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.width = width;
    window.height = height;
    window.minimize = width == 0 || height == 0;

    glm::ivec2 size {width, height};

    window.FramebufferEvent(size);
}

void Window::CursorPosCallback(GLFWwindow* handle, double posX, double posY) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    glm::vec2 pos {posX, posY};

    window.MouseMotionEvent(pos);

    glm::vec2 norm {
        2.0f * pos.x / static_cast<float>(window.width - 1),
        2.0f * pos.y / static_cast<float>(window.height - 1)
    };

    window.MouseMotionNormEvent(norm);
}

void Window::ScrollCallback(GLFWwindow* handle, double offsetX, double offsetY) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    glm::vec2 offset {offsetX, offsetY};

    window.MouseScrollEvent(offset);
}

void Window::MouseButtonCallback(GLFWwindow* handle, int button, int action, int mode) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.MouseButtonEvent(button, action, mode);

    switch (action) {
        case GLFW_PRESS:
            window.MousePressEvent(button);
            break;
        case GLFW_RELEASE:
            window.MouseReleaseEvent(button);
            break;
    }
}

void Window::KeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mode) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.KeyEvent(key, scancode, action, mode);

    switch (action) {
        case GLFW_PRESS:
            window.KeyPressEvent(key);
            break;
        case GLFW_RELEASE:
            window.KeyReleaseEvent(key);
            break;
        case GLFW_REPEAT:
            window.KeyHoldEvent(key);
            break;
    }
}

void Window::CursorEnterCallback(GLFWwindow* handle, int entered) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.MouseEnterEvent(entered);
}

void Window::CharCallback(GLFWwindow* handle, unsigned int keycode) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.CharInputEvent(keycode);
}

#if GLFW_VERSION_MINOR >= 1
void Window::FileDropCallback(GLFWwindow* handle, int count, const char** paths) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    std::vector<std::string> result;
    result.reserve(count);

    for (int i = 0; i < count; ++i) {
        result.emplace_back(paths[i]);
    }

    window.FileDropEvent(result);
}
#endif

#if GLFW_VERSION_MINOR >= 2
void Window::JoystickCallback(int jid, int action) {
    for (auto* handle : instances) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        switch (action) {
            case GLFW_CONNECTED:
                window.JoystickEvent(jid, true);
                break;
            case GLFW_DISCONNECTED:
                window.JoystickEvent(jid, false);
                break;
        }
    }

}
#endif

#if GLFW_VERSION_MINOR >= 3
void Window::MaximizeCallback(GLFWwindow* handle, int maximized) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.minimize = !maximized;

    window.MaximizeEvent(maximized);
}

void Window::ContentScaleCallback(GLFWwindow* handle, float scaleX, float scaleY) {
    auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

    glm::vec2 scale {scaleX, scaleY};

    window.ContentScaleEvent(scale);
}
#endif

void Window::MonitorCallback(GLFWmonitor* monitor, int action) {
    for (auto* handle : instances) {
        auto& window = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));

        switch (action) {
            case GLFW_CONNECTED:
                window.MonitorEvent(true);
                break;
            case GLFW_DISCONNECTED:
                window.MonitorEvent(false);
                break;
        }
    }
}

void Window::ErrorCallback(int error, const char* description) {
    FE_LOG_ERROR << "[GLFW] Error (" << error << "): " << description;
}
#endif