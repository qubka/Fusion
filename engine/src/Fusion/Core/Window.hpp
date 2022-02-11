#pragma once

#include "GLFW/glfw3.h"

#include "Fusion/Events/Event.hpp"

namespace Fusion {
    class FUSION_API Window {
    public:
        Window(std::string title, int width, int height, bool vsync);
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

        bool vSync() const  { return vsync; }
        void vSync(bool flag);

        bool isMinimize() const { return minimize; }
        EventBus& bus() { return eventBus; }

    private:
        GLFWwindow* window;
        std::string title;
        int width;
        int height;
        float aspect;
        bool vsync;
        bool minimize;

        EventBus eventBus;

        void init();

        static uint8_t GLFWwindowCount;
        static void WindowResizeCallback(GLFWwindow* handle, int width, int height);
        static void WindowCloseCallback(GLFWwindow* handle);
    };
}
