#pragma once
#if !defined(ANDROID)

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "fusion/events/event_queue.hpp"

namespace Fusion {
    class Window {
    public:
        Window(std::string title, const glm::uvec2& size, const glm::ivec2& position = {}, bool fullscreen = false);
        ~Window();
        FE_NONCOPYABLE(Window);

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
                frameHandler();
            }
        }

        bool isMinimize() const { return minimize; }
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

        static void PosCallback(GLFWwindow* handle, int x, int y);
        static void SizeCallback(GLFWwindow* handle, int width, int height);
        static void CloseCallback(GLFWwindow* handle);
        static void RefreshCallback(GLFWwindow* handle);
        static void FocusCallback(GLFWwindow* handle, int focused);
        static void IconifyCallback(GLFWwindow* handle, int iconified);
        static void FramebufferSizeCallback(GLFWwindow* handle, int width, int height);
#if GLFW_VERSION_MINOR >= 1
        static void FileDropCallback(GLFWwindow* handle, int count, const char** paths);
#endif
#if GLFW_VERSION_MINOR >= 2
        static void JoystickCallback(int jid, int action);
#endif
#if GLFW_VERSION_MINOR >= 3
        static void MaximizeCallback(GLFWwindow* handle, int maximized);
        static void ContentScaleCallback(GLFWwindow* handle, float scaleX, float scaleY);
#endif
        static void ErrorCallback(int error, const char* description);
        static void MonitorCallback(GLFWmonitor* monitor, int action);
    };
}
#endif
