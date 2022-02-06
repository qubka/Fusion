#pragma once

#include "Core.hpp"

namespace Fusion {
    class Event;

    struct WindowProps {
        std::string title{"Fusion Engine"};
        uint32_t width{1280};
        uint32_t height{720};
    };

    class FUSION_API Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() = default;

        virtual void onUpdate() = 0;

        virtual std::string getTitle() = 0;
        virtual uint32_t getWidth() = 0;
        virtual uint32_t getHeight() = 0;
        virtual float getAspect() = 0;
        virtual glm::vec4 getViewport() = 0;

        virtual void setEventCallback(const EventCallbackFn& callback) = 0;

        static Window* create(const WindowProps& props = WindowProps{});
    };
}