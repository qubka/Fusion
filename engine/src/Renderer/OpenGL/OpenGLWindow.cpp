#include "OpenGLWindow.hpp"

using namespace Fusion;

Window* Window::create(const WindowInfo& info) {
    return new OpenGLWindow(info);
}

OpenGLWindow::OpenGLWindow(const WindowInfo& info) {
    FS_CORE_ASSERT(info.width > 0 && info.height > 0, "width or height cannot be negative");
    data.title = info.title;
    data.width = info.width;
    data.height = info.height;
    data.vsync = info.vsync;

    init();
}

OpenGLWindow::~OpenGLWindow() {
    glfwDestroyWindow(window);
}

void OpenGLWindow::init() {
    FS_LOG_CORE_INFO("Creating window: {0} ({1}, {2})", data.title, data.width, data.height);

    window = glfwCreateWindow(static_cast<int>(data.width), static_cast<int>(data.height), data.title.c_str(), nullptr, nullptr);
    FS_CORE_ASSERT(window, "failed to create window!");
    glfwMakeContextCurrent(window);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    FS_CORE_ASSERT(status, "failed to initialize GLAD!");
    glfwSwapInterval(data.vsync);
    glfwSetWindowUserPointer(window, &data);

    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        auto& data = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));
        data.width = static_cast<uint32_t>(width);
        data.height = static_cast<uint32_t>(height);
        data.aspect = static_cast<float>(width) / static_cast<float>(height);

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
        MouseMovedEvent event{{mouseX, mouseY}};
        data.eventCallback(event);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double offsetX, double offsetY)
    {
        auto& data = *reinterpret_cast<WindowData *>(glfwGetWindowUserPointer(window));
        MouseScrollEvent event{{offsetX, offsetY}};
        data.eventCallback(event);
    });
}

void OpenGLWindow::onUpdate() {
    glfwPollEvents();
    glfwSwapBuffers(window);
}

void OpenGLWindow::vSync(bool flag) {
    glfwSwapInterval(flag);
    data.vsync = flag;
}
