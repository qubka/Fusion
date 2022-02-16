#pragma once

#include "EventQueue.hpp"
#include "Fusion/Input/MouseCodes.hpp"

namespace Fusion {

    struct FUSION_API MouseMovedEvent : public Event {
        glm::vec2 mouse;
    };

    struct FUSION_API MouseScrollEvent : public Event {
        glm::vec2 offset;
    };

    struct FUSION_API MouseCursorEnterEvent : public Event {
    };

    struct FUSION_API MouseCursorLeftEvent : public Event {
    };

    struct FUSION_API MouseButtonEvent : public Event {
        MouseCode button;
    };

    struct FUSION_API MouseButtonPressedEvent : public MouseButtonEvent {
    };

    struct FUSION_API MouseButtonReleasedEvent : public MouseButtonEvent {
    };
}