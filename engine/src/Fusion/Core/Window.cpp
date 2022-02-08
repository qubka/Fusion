#include "Window.hpp"
#include "Log.hpp"

using namespace Fusion;

uint8_t Window::GLFWWindowCount{0};

static void GLFWErrorCallback(int error, const char* description) {
    FS_LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Window::Window(std::string title, uint32_t width, uint32_t height, bool vsync) :
    title{std::move(title)},
    width{width},
    height{height},
    aspect{static_cast<float>(width) / static_cast<float>(height)},
    vsync{vsync}
{
    FS_CORE_ASSERT(width > 0 && height > 0, "Width or height cannot be negative");

    if (GLFWWindowCount == 0) {
        int success = glfwInit();
        FS_CORE_ASSERT(success, "Could not initialize GLFW!");
        glfwSetErrorCallback(GLFWErrorCallback);
    }

    init();
}

Window::~Window() {
    glfwDestroyWindow(window);

    --GLFWWindowCount;
    if (GLFWWindowCount == 0) {
        glfwTerminate();
    }
}

void Window::init() {
    FS_LOG_CORE_INFO("Creating window: {0} ({1}, {2})", title, width, height);

#ifdef GLFW_INCLUDE_VULKAN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#endif

    window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
    FS_CORE_ASSERT(window, "failed to create window!");
    GLFWWindowCount++;

#ifndef GLFW_INCLUDE_VULKAN
    glfwMakeContextCurrent(window);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    FS_CORE_ASSERT(status, "failed to initialize GLAD!");
    glfwSwapInterval(vsync);
#endif

    glfwSetWindowUserPointer(window, this);

#ifdef GLFW_INCLUDE_VULKAN
    glfwSetFramebufferSizeCallback(window, WindowResizeCallback);
#else
    glfwSetWindowSizeCallback(window, WindowResizeCallback);
#endif
}

void Window::onUpdate() {
    glfwPollEvents();
#ifndef GLFW_INCLUDE_VULKAN
    glfwSwapBuffers(window);
#endif
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::shouldClose(bool flag) const {
    glfwSetWindowShouldClose(window, flag);
}

void Window::vSync(bool flag) {
#ifndef GLFW_INCLUDE_VULKAN
    glfwSwapInterval(flag);
#endif
    vsync = flag;
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
    window.width = static_cast<uint32_t>(width);
    window.height = static_cast<uint32_t>(height);
    window.aspect = static_cast<float>(width) / static_cast<float>(height);
#ifdef GLFW_INCLUDE_VULKAN
    window.resized = true;
#else
    glViewport(0, 0, width, height);
#endif
}