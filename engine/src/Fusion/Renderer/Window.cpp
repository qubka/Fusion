#include "Window.hpp"

#include "Fusion/Events/WindowEvents.hpp"

using namespace Fusion;

uint8_t Window::GLFWwindowCount{0};

static void GLFWErrorCallback(int error, const char* description) {
    FE__LOG_ERROR << "[GLFW] Error (" << error << "): " << description;
}

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
        glfwSetErrorCallback(GLFWErrorCallback);
    }

    init();
}

Window::~Window() {
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

    glfwSetFramebufferSizeCallback(window, WindowResizeCallback);
    //glfwSetWindowCloseCallback(window, WindowCloseCallback);
}

void Window::onUpdate() const {
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

void Window::WindowResizeCallback(GLFWwindow* handle, int width, int height) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.width = width;
    window.height = height;
    window.aspect = static_cast<float>(width) / static_cast<float>(height);
    window.minimize = width == 0 || height == 0;

    window.bus().publish(new WindowResizeEvent{});
}

void Window::WindowCloseCallback(GLFWwindow* handle) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.bus().publish(new WindowCloseEvent{});
}
