#pragma once

#include "Event.hpp"

namespace Fusion {
    class FUSION_API WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose);
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class FUSION_API WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height)
            : width{width}, height{height} {}

        uint32_t getWidth() const { return width; }
        uint32_t getHeight() const { return height; }

        std::string toString() const override {
            return "WindowResizeEvent: [" + std::to_string(width) + ", " + std::to_string(height) + "]";
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    private:
        uint32_t width;
        uint32_t height;
    };

}
