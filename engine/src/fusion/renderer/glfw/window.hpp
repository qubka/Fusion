#pragma once
#if !defined(ANDROID)

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "fusion/events/event_queue.hpp"
#include "fusion/input/input.hpp"

namespace Fusion {
    class Window {
    public:
        Window(std::string title, const glm::uvec2& size, const glm::ivec2& position = {}, bool fullscreen = false);
        ~Window();

        operator GLFWwindow*() const { return window; }
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        glm::uvec2 getSize() const { return {width, height}; }
        float getAspect() const { return aspect; }
        const std::string& getTitle() const { return title; }
        glm::vec4 getViewport() const;

        bool shouldClose() const { return glfwWindowShouldClose(window); };
        void shouldClose(bool flag) const { glfwSetWindowShouldClose(window, flag); };
        void makeCurrent() const { glfwMakeContextCurrent(window); }
        void swapBuffers() const { glfwSwapBuffers(window); }

        void showWindow(bool show = true) {
            if (show) {
                glfwShowWindow(window);
            } else {
                glfwHideWindow(window);
            }
        }

        void setTitle(const std::string& str) {
            title = str;
            glfwSetWindowTitle(window, title.c_str());
        }

        void setSizeLimits(const glm::uvec2& minSize, const glm::uvec2& maxSize = {}) {
            glfwSetWindowSizeLimits(window, minSize.x, minSize.y, (maxSize.x != 0) ? maxSize.x : minSize.x, (maxSize.y != 0) ? maxSize.y : minSize.y);
        }

        void runWindowLoop(const std::function<void()>& frameHandler) {
            while (!shouldClose()) {
                eventQueue.free();
                glfwPollEvents();
                Input::Update();
                frameHandler();
            }
        }

        bool isMinimize() const { return minimize; }
        bool isFullscreen() const { return fullscreen; }
        EventQueue& getEventQueue() { return eventQueue; }

#if defined(VULKAN_HPP)
        static std::vector<std::string> getRequiredInstanceExtensions();
        static vk::SurfaceKHR createWindowSurface(GLFWwindow* window, const vk::Instance& instance, const vk::AllocationCallbacks* pAllocator = nullptr);
        vk::SurfaceKHR createSurface(const vk::Instance& instance, const vk::AllocationCallbacks* pAllocator = nullptr) {
            return createWindowSurface(window, instance, pAllocator);
        }
        vk::Extent2D getExtent() const { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
#endif

    private:
        GLFWwindow* window;
        std::string title;
        int width;
        int height;
        float aspect;
        bool minimize;
        bool fullscreen;

        EventQueue eventQueue;

        void initWindow(const glm::ivec2& position);

        static uint8_t GLFWwindowCount;
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
