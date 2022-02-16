#pragma once

#include "glfw/include/GLFW/glfw3.h"

#include "Fusion/Events/EventQueue.hpp"

namespace Fusion {
    class FUSION_API Window {
    public:
        Window(std::string title, int width, int height);
        ~Window();

        void onUpdate();

        operator GLFWwindow*() const { return window; }
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        float getAspect() const { return aspect; }
        const std::string& getTitle() const { return title; }
        glm::vec4 getViewport() const;

        bool shouldClose() const;
        void shouldClose(bool flag) const;

        bool isMinimize() const { return minimize; }
        EventQueue& getEventQueue() { return eventQueue; }

    private:
        GLFWwindow* window;
        std::string title;
        int width;
        int height;
        float aspect;
        bool minimize;

        EventQueue eventQueue;

        void init();

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
