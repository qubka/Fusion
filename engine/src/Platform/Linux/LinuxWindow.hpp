#pragma once

#include "../../Fusion.hpp"

class GLFWwindow;

namespace Fusion {
    class LinuxWindow : public Window {
    public:
        LinuxWindow(const WindowProps& props);
        virtual ~LinuxWindow();

        void onUpdate() override;

        std::string getTitle() override { return data.title; }
        uint32_t getWidth() override { return data.width; };
        uint32_t getHeight() override { return data.height; };
        float getAspect() override { return data.aspect; };
        glm::vec4 getViewport() override;

        bool wasResized() const;
        void resetResized();

        void setEventCallback(const EventCallbackFn& callback) override { data.eventCallback = callback; };
    private:
        GLFWwindow* window;

        struct WindowData {
            std::string title;
            uint32_t width;
            uint32_t height;
            float aspect;
            bool resized;
            bool locked;
            EventCallbackFn eventCallback;
        };

        WindowData data;

        static bool GLFWInitialized;
    };
}
