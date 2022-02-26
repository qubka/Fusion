#pragma once

namespace fe {
    struct MouseData {
        uint8_t button;
        uint8_t action;
        uint8_t mods;
    };

    using MouseCode = uint8_t;

    namespace Mouse {
        // From glfw3.h
        enum : MouseCode {
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