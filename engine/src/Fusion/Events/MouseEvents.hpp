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
        MouseMovedEvent(const glm::vec2 mouse) : mouse{mouse} {}

        const glm::vec2& getMouse() const { return mouse; }

        std::string toString() const override {
            return "MouseMovedEvent: " + glm::to_string(mouse);
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        glm::vec2 mouse;
    };

    class FUSION_API MouseScrollEvent : public Event {
    public:
        MouseScrollEvent(const glm::vec2& offset) : offset{offset} {}

        const glm::vec2& getOffset() const { return offset; }

        std::string toString() const override {
            return "MouseScrollEvent: " + glm::to_string(offset);
        }

        EVENT_CLASS_TYPE(MouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        glm::vec2 offset;
    };
}