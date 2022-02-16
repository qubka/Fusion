#pragma once

#include "EventQueue.hpp"
#include "Fusion/Input/KeyCodes.hpp"

namespace Fusion {

    struct FUSION_API KeyEvent : public Event {
        KeyCode keycode;
    };

    struct FUSION_API KeyPressedEvent : public KeyEvent {
        bool repeat;
    };

    struct FUSION_API KeyReleasedEvent : public KeyEvent {
    };

    struct FUSION_API KeyTypedEvent : public KeyEvent {
    };
}