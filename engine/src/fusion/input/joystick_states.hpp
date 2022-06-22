#pragma once

#include "action_codes.hpp"

namespace fe {
    using JoystickButton = uint8_t;
    using JoystickAxis = uint8_t;
    using JoystickHat = uint8_t;

    struct JoyButtonData {
        JoystickButton button;
        ActionCode action;
    };

    struct JoyAxisData {
        JoystickAxis axis;
        float value;
    };

    struct JoyHatData {
        JoystickHat hat;
        JoystickHat mask;
    };

    namespace Joy {
        // From glw3.h
        enum : JoystickButton {
            Button0              = 0,
            Button1              = 1,
            Button2              = 2,
            Button3              = 3,
            Button4              = 4,
            Button5              = 5,
            Button6              = 6,
            Button7              = 7,
            Button8              = 8,
            Button9              = 9,
            Button10             = 10,
            Button11             = 11,
            Button12             = 12,
            Button13             = 13,
            Button14             = 14,
            Button15             = 15,
            Button16             = 16,
        };
    }

    namespace JoyHat {
        // From glfw3.h
        enum : JoystickHat {
            Centered             = 0,
            Up                   = 1,
            Right                = 2,
            Down                 = 4,
            Left                 = 8,
            RightUp              = Right | Up,
            RightDown            = Right | Down,
            LeftUp               = Left | Up,
            LeftDown             = Left | Down,
        };
    }

    namespace Gamepad {
        // From glw3.h
        enum : JoystickButton {
            ButtonA              = 0,
            ButtonB              = 1,
            ButtonX              = 2,
            ButtonY              = 3,
            ButtonLeftBumper     = 4,
            ButtonRightBumper    = 5,
            ButtonBack           = 6,
            ButtonStart          = 7,
            ButtonGuide          = 8,
            ButtonLeftThumb      = 9,
            ButtonRightThumb     = 10,
            ButtonDPadUp         = 11,
            ButtonDPadRight      = 12,
            ButtonDPadDown       = 13,
            ButtonDPadLeft       = 14,

            ButtonCross          = ButtonA,
            ButtonCircle         = ButtonB,
            ButtonSquare         = ButtonX,
            ButtonTriangle       = ButtonY
        };
    }

    namespace GamepadAxis {
        // From glw3.h
        enum : JoystickAxis {
            LeftX               = 0,
            LeftY               = 1,
            RightX              = 2,
            RightY              = 3,
            LeftTrigger         = 4,
            RightTrigger        = 5
        };
    }
}