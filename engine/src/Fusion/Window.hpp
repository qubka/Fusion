#pragma once

#include "Core.hpp"

namespace Fusion {
    struct WindowInfo {
        std::string title{"Fusion Engine"};
        uint32_t width{1280};
        uint32_t height{720};
        bool vsync{true};
    };

    class FUSION_API Window {
    public:
        Window() = default;
        virtual ~Window() = default;

        virtual void onUpdate() = 0;

        virtual void* getNativeWindow() = 0;
        virtual std::string getTitle() = 0;
        virtual uint32_t getWidth() = 0;
        virtual uint32_t getHeight() = 0;
        virtual float getAspect() = 0;
        virtual glm::vec4 getViewport() = 0;

        virtual bool shouldClose() = 0;
        virtual void shouldClose(bool flag) = 0;

        virtual bool wasResized() { return false; };
        virtual void resetResized() {};

        virtual bool vSync() { return true; };
        virtual void vSync(bool flag) {};

        static Window* create(const WindowInfo& info = WindowInfo{});
    };
}