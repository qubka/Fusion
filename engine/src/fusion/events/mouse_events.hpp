#pragma once

#include "event_queue.hpp"
#include "Fusion/Input/MouseCodes.hpp"

namespace Fusion {

    struct  MouseMovedEvent : public Event {
        glm::vec2 mouse;
    };

    struct  MouseScrollEvent : public Event {
        glm::vec2 offset;
    };

    struct  MouseCursorEnterEvent : public Event {
    };

    struct  MouseCursorLeftEvent : public Event {
    };

    struct  MouseButtonEvent : public Event {
        MouseCode button;
    };

    struct  MouseButtonPressedEvent : public MouseButtonEvent {
    };

    struct  MouseButtonReleasedEvent : public MouseButtonEvent {
    };
}