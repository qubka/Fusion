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
        auto& data = *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
        data.width = static_cast<uint32_t>(width);
        data.height = static_cast<uint32_t>(height);
        data.aspect = static_cast<float>(width) / static_cast<float>(height);
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

bool OpenGLWindow::shouldClose() {
    return glfwWindowShouldClose(window);
}

void OpenGLWindow::shouldClose(bool flag) {
    glfwSetWindowShouldClose(window, flag);
}
