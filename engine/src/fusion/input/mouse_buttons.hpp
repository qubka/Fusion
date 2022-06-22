#pragma once

#include "action_codes.hpp"
#include "key_codes.hpp"

namespace fe {
    using MouseButton = uint8_t;

    struct MouseData {
        MouseButton button;
        ActionCode action;
        ModCode mods;
    };

    namespace Mouse {
        // From glfw3.h
        enum : MouseButton {
            Button0                = 0,
            Button1                = 1,
            Button2                = 2,
            Button3                = 3,
            Button4                = 4,
            Button5                = 5,
            Button6                = 6,
            Button7                = 7,

            ButtonLast             = Button7,
            ButtonLeft             = Button0,
            ButtonRight            = Button1,
            ButtonMiddle           = Button2,
        };
    }
}