#pragma once

#include "Event.hpp"

namespace Fusion {
    class FUSION_API KeyEvent : public Event {
        int geyKeyCode() const { return keycode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

    protected:
        KeyEvent(int keycode) : keycode(keycode) {}
        int keycode;
    };

    class FUSION_API KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(int keycode, int repeatCount) : KeyEvent{keycode}, repeatCount{repeatCount} {}
        int getRepeatCount() const { return repeatCount; }

        std::string toString() const override {
            return "KeyPressedEvent: " + std::to_string(keycode) + " (" + std::to_string(repeatCount) + ") repeats";
        }

        EVENT_CLASS_TYPE(KeyPressed)

    private:
        int repeatCount;
    };

    class FUSION_API KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(int keycode) : KeyEvent{keycode} {}

        std::string toString() const override {
            return "KeyReleasedEvent: " + std::to_string(keycode);
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };
}