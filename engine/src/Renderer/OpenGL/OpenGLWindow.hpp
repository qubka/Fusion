#pragma once

#include "../../Fusion.hpp"

class GLFWwindow;

namespace Fusion {
    class FUSION_API OpenGLWindow : public Window {
    public:
        OpenGLWindow(const WindowInfo& info);
        virtual ~OpenGLWindow();

        void onUpdate() override;

        void* getNativeWindow() override { return window; };
        std::string getTitle() override { return data.title; }
        uint32_t getWidth() override { return data.width; };
        uint32_t getHeight() override { return data.height; };
        float getAspect() override { return data.aspect; };
        glm::vec4 getViewport() override { return {0, data.height, data.width, -data.height}; }; // vertical flip is required

        bool isVSync() override { return data.vsync; };
        void vSync(bool flag) override;

        void setEventCallback(const EventCallbackFn& callback) override { data.eventCallback = callback; };
    private:
        void init();

        GLFWwindow* window;

        struct WindowData {
            std::string title;
            uint32_t width;
            uint32_t height;
            float aspect;
            bool vsync;
            EventCallbackFn eventCallback;
        };

        WindowData data;
    };
}
