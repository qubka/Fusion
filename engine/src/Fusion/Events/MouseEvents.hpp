#pragma once

#include "Event.hpp"

namespace Fusion {
    class FUSION_API MouseButtonEvent : public Event {
        int geyButton() const { return button; }

        EVENT_CLASS_CATEGORY(EventCategoryMouseButton | EventCategoryInput)

    protected:
        MouseButtonEvent(int button) : button(button) {}
        int button;
    };

    class FUSION_API MouseButtonPressedEvent : public MouseButtonEvent {
    public:
        MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

        std::string toString() const override {
            return "MouseButtonPressedEvent: " + std::to_string(button);
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class FUSION_API MouseButtonReleasedEvent : public MouseButtonEvent {
    public:
        MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

        std::string toString() const override {
            return "MouseButtonReleasedEvent: " + std::to_string(button);
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };

    class FUSION_API MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(float x, float y) : mouseX{x}, mouseY{y} {}

        float getX() const { return mouseX; }
        float getY() const { return mouseY; }

        std::string toString() const override {
            return "MouseMovedEvent: [" + std::to_string(mouseX) + ", " + std::to_string(mouseY) + "]";
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        float mouseX;
        float mouseY;
    };

    class FUSION_API MouseScrollEvent : public Event {
    public:
        MouseScrollEvent(float x, float y) : offsetX{x}, offsetY{y} {}

        float getX() const { return offsetX; }
        float getY() const { return offsetY; }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        float offsetX;
        float offsetY;
    };
}