#include "LinuxWindow.hpp"
#include "GLFW/glfw3.h"

using namespace Fusion;

bool LinuxWindow::GLFWInitialized{false};

static void GLFWErrorCallback(int error, const char* description) {
    FS_LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

Window* Window::create(const WindowProps& props) {
    return new LinuxWindow(props);
}

LinuxWindow::LinuxWindow(const WindowProps& props) {
    FS_CORE_ASSERT(props.width > 0 && props.height > 0, "Width or height cannot be negative");
    data.title = props.title;
    data.width = props.width;
    data.height = props.height;

    FS_LOG_CORE_INFO("Creating window {0} ({1}, {2})", data.title, data.width, data.height);

    if (!GLFWInitialized) {
        int success = glfwInit();
        FS_CORE_ASSERT(success, "Could not initialize GLFW!");
        glfwSetErrorCallback(GLFWErrorCallback);
        GLFWInitialized = true;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(static_cast<int>(data.width), static_cast<int>(data.height), data.title.c_str(), nullptr, nullptr);

    FS_CORE_ASSERT(window, "failed to create window!");

    //glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, &data);

#ifdef GLFW_INCLUDE_VULKAN
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
#else
    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
#endif
    {
        auto& data = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));
        data.width = static_cast<uint32_t>(width);
        data.height = static_cast<uint32_t>(height);
        data.aspect = static_cast<float>(width) / static_cast<float>(height);
        data.resized = true;

        WindowResizeEvent event{data.width, data.height};
        data.eventCallback(event);
    });
    glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
    {
        auto& data = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));
        WindowCloseEvent event{};
        data.eventCallback(event);
    });
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        auto& data = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event{key, 0};
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event{key};
                data.eventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event{key, 1};
                data.eventCallback(event);
                break;
            }
        }
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mode)
    {
        auto& data = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                MouseButtonPressedEvent event{button};
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                MouseButtonReleasedEvent event{button};
                data.eventCallback(event);
                break;
            }
        }
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double mouseX, double mouseY)
    {
        auto& data = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));
        MouseMovedEvent event{static_cast<float>(mouseX), static_cast<float>(mouseY)};
        data.eventCallback(event);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double offsetX, double offsetY)
    {
        auto& data = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));
        MouseScrollEvent event{static_cast<float>(offsetX), static_cast<float>(offsetY)};
        data.eventCallback(event);
    });
}

LinuxWindow::~LinuxWindow() {
    glfwDestroyWindow(window);
}

glm::vec4 LinuxWindow::getViewport() {
#ifdef GLFW_INCLUDE_VULKAN
    return {0, 0, width, height};
#else // OPENGL
    return {0, data.height, data.width, -data.height}; // vertical flip is required
#endif
}

bool LinuxWindow::wasResized() const {
    return data.resized;
}

void LinuxWindow::resetResized() {
    data.resized = false;
}

void LinuxWindow::onUpdate() {
    glfwPollEvents();
    //glfwSwapBuffers(window);
}