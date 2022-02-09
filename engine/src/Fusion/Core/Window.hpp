#pragma once

#include "Base.hpp"

namespace Fusion {
    class FUSION_API Window {
    public:
        Window(std::string title, uint32_t width, uint32_t height, bool vsync);
        ~Window();

        void onUpdate();

        operator GLFWwindow*() const { return window; }
        uint32_t getWidth() const { return width; }
        uint32_t getHeight() const { return height; }
        float getAspect() const { return aspect; }
        const std::string& getTitle() const { return title; }
        glm::vec4 getViewport() const;

        bool vSync() const  { return vsync; }
        void vSync(bool flag);

        bool wasResized() const { return resized; };
        void resetResized() { resized = false; };

        bool shouldClose() const;
        void shouldClose(bool flag) const;

    private:
        GLFWwindow* window;
        std::string title;
        uint32_t width;
        uint32_t height;
        float aspect;
        bool vsync;
        bool resized;
        bool locked;

        //EventBus eventBus;

        void init();

        static uint8_t GLFWWindowCount;
        static void WindowResizeCallback(GLFWwindow* handle, int width, int height);
    };
}
