#pragma once

#include "event_queue.hpp"
#include "Fusion/Input/KeyCodes.hpp"

namespace Fusion {

    struct  KeyEvent : public Event {
        KeyCode keycode;
    };

    struct  KeyPressedEvent : public KeyEvent {
        bool repeat;
    };

    struct  KeyReleasedEvent : public KeyEvent {
    };

    struct  KeyTypedEvent : public KeyEvent {
    };
}