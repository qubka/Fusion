#pragma once
#if !defined(ANDROID)

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "fusion/core/window.hpp"
#include "fusion/input/key_input.hpp"
#include "fusion/input/mouse_input.hpp"

namespace glfw {
    class Window : public fe::Window {
    public:
        Window(std::string title, const glm::ivec2& size, const glm::ivec2& position = {});
        Window(std::string title);
        ~Window() override;

        void* getNativeWindow() override { return window; }
        bool shouldClose() override { return glfwWindowShouldClose(window); };
        void shouldClose(bool flag) override { glfwSetWindowShouldClose(window, flag); };

        void makeCurrent() const { glfwMakeContextCurrent(window); }
        void swapBuffers() const { glfwSwapBuffers(window); }

        void showWindow(bool show = true) {
            if (show) {
                glfwShowWindow(window);
            } else {
                glfwHideWindow(window);
            }
        }

        const std::string& getTitle() const { return title; }
        void setTitle(const std::string& str) {
            title = str;
            glfwSetWindowTitle(window, title.c_str());
        }

        const glm::ivec2& getPosition() const { return position; }
        void setSizeLimits(const glm::uvec2& minSize, const glm::uvec2& maxSize = {}) {
            glfwSetWindowSizeLimits(window, minSize.x, minSize.y, (maxSize.x != 0) ? maxSize.x : minSize.x, (maxSize.y != 0) ? maxSize.y : minSize.y);
        }

        void pollEvents() override {
            glfwPollEvents();
        }

#if defined(VULKAN_HPP)
        static std::vector<std::string> getRequiredInstanceExtensions();
        static vk::SurfaceKHR createWindowSurface(GLFWwindow* window, const vk::Instance& instance, const vk::AllocationCallbacks* pAllocator = nullptr);
        vk::SurfaceKHR createSurface(const vk::Instance& instance, const vk::AllocationCallbacks* pAllocator = nullptr) {
            return createWindowSurface(window, instance, pAllocator);
        }
#endif

    private:
        GLFWwindow* window;
        std::string title;
        glm::ivec2 position;

        void initGLFW();
        void initWindow(bool fullscreen);

        static std::vector<GLFWwindow*> instances;

        /// Workaround for C++ class using a c-style-callback
        static void PosCallback(GLFWwindow* window, int x, int y);
        static void SizeCallback(GLFWwindow* window, int width, int height);
        static void CloseCallback(GLFWwindow* window);
        static void RefreshCallback(GLFWwindow* window);
        static void FocusCallback(GLFWwindow* window, int focused);
        static void IconifyCallback(GLFWwindow* window, int iconified);
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
        static void CursorPosCallback(GLFWwindow* window, double posX, double posY);
        static void CursorEnterCallback(GLFWwindow* window, int entered);
        static void ScrollCallback(GLFWwindow* window, double offsetX, double offsetY);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void CharCallback(GLFWwindow* window, unsigned int keycode);
#if GLFW_VERSION_MINOR >= 1
        static void FileDropCallback(GLFWwindow* window, int count, const char** paths);
#endif
#if GLFW_VERSION_MINOR >= 2
        static void JoystickCallback(int jid, int action);
#endif
#if GLFW_VERSION_MINOR >= 3
        static void MaximizeCallback(GLFWwindow* window, int maximized);
        static void ContentScaleCallback(GLFWwindow* window, float scaleX, float scaleY);
#endif
        static void ErrorCallback(int error, const char* description);
        static void MonitorCallback(GLFWmonitor* monitor, int action);
    };
}
#endif
