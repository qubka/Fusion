#pragma once

namespace fe {
    enum class InputAction : unsigned char {
        Release                 = 0,  /** The key or mouse button was released. */
        Press                   = 1,  /** The key or mouse button was pressed. */
        Repeat                  = 2,  /** The key was held down until it repeated. */
    };
    inline bool operator!(InputAction action) { return action == InputAction::Release; }

    enum class InputMod : unsigned char {
        Shift                   = 1,  /** If this bit is set one or more Shift keys were held down. */
        Control                 = 2,  /** If this bit is set one or more Control keys were held down. */
        Alt                     = 4,  /** If this bit is set one or more Alt keys were held down. */
        Super                   = 8,  /** If this bit is set one or more Super keys were held down. */
        CapsLock                = 16, /** If this bit is set the Caps Lock key is enabled. */
        NumLock                 = 32, /** If this bit is set the Num Lock key is enabled. */
    };
    BITMASK_DEFINE_MAX_ELEMENT(InputMod, NumLock);

    enum class Key : int16_t {
        Space                   = 32,
        Apostrophe              = 39, /* ' */
        Comma                   = 44, /* , */
        Minus                   = 45, /* - */
        Period                  = 46, /* . */
        Slash                   = 47, /* / */

        D0                      = 48, /* 0 */
        D1                      = 49, /* 1 */
        D2                      = 50, /* 2 */
        D3                      = 51, /* 3 */
        D4                      = 52, /* 4 */
        D5                      = 53, /* 5 */
        D6                      = 54, /* 6 */
        D7                      = 55, /* 7 */
        D8                      = 56, /* 8 */
        D9                      = 57, /* 9 */

        Semicolon               = 59, /* ; */
        Equal                   = 61, /* = */

        A                       = 65,
        B                       = 66,
        C                       = 67,
        D                       = 68,
        E                       = 69,
        F                       = 70,
        G                       = 71,
        H                       = 72,
        I                       = 73,
        J                       = 74,
        K                       = 75,
        L                       = 76,
        M                       = 77,
        N                       = 78,
        O                       = 79,
        P                       = 80,
        Q                       = 81,
        R                       = 82,
        S                       = 83,
        T                       = 84,
        U                       = 85,
        V                       = 86,
        W                       = 87,
        X                       = 88,
        Y                       = 89,
        Z                       = 90,

        LeftBracket             = 91,  /* [ */
        Backslash               = 92,  /* \ */
        RightBracket            = 93,  /* ] */
        GraveAccent             = 96,  /* ` */

        World1                  = 161, /* non-US #1 */
        World2                  = 162, /* non-US #2 */

        /* Function keys */
        Escape                  = 256,
        Enter                   = 257,
        Tab                     = 258,
        Backspace               = 259,
        Insert                  = 260,
        Delete                  = 261,
        Right                   = 262,
        Left                    = 263,
        Down                    = 264,
        Up                      = 265,
        PageUp                  = 266,
        PageDown                = 267,
        Home                    = 268,
        End                     = 269,
        CapsLock                = 280,
        ScrollLock              = 281,
        NumLock                 = 282,
        PrintScreen             = 283,
        Pause                   = 284,
        F1                      = 290,
        F2                      = 291,
        F3                      = 292,
        F4                      = 293,
        F5                      = 294,
        F6                      = 295,
        F7                      = 296,
        F8                      = 297,
        F9                      = 298,
        F10                     = 299,
        F11                     = 300,
        F12                     = 301,
        F13                     = 302,
        F14                     = 303,
        F15                     = 304,
        F16                     = 305,
        F17                     = 306,
        F18                     = 307,
        F19                     = 308,
        F20                     = 309,
        F21                     = 310,
        F22                     = 311,
        F23                     = 312,
        F24                     = 313,
        F25                     = 314,

        /* Keypad */
        KP0                     = 320,
        KP1                     = 321,
        KP2                     = 322,
        KP3                     = 323,
        KP4                     = 324,
        KP5                     = 325,
        KP6                     = 326,
        KP7                     = 327,
        KP8                     = 328,
        KP9                     = 329,
        KPDecimal               = 330,
        KPDivide                = 331,
        KPMultiply              = 332,
        KPSubtract              = 333,
        KPAdd                   = 334,
        KPEnter                 = 335,
        KPEqual                 = 336,

        LeftShift               = 340,
        LeftControl             = 341,
        LeftAlt                 = 342,
        LeftSuper               = 343,
        RightShift              = 344,
        RightControl            = 345,
        RightAlt                = 346,
        RightSuper              = 347,
        Menu                    = 348,

        KeyLast                 = Menu
    };

    enum class MouseButton : unsigned char {
        Button0                 = 0,
        Button1                 = 1,
        Button2                 = 2,
        Button3                 = 3,
        Button4                 = 4,
        Button5                 = 5,
        Button6                 = 6,
        Button7                 = 7,

        ButtonLast              = Button7,
        ButtonLeft              = Button0,
        ButtonRight             = Button1,
        ButtonMiddle            = Button2,
    };

    enum class JoystickHat : unsigned char {
        Centered                = 0,
        Up                      = 1,
        Right                   = 2,
        Down                    = 4,
        Left                    = 8,
    };
    BITMASK_DEFINE_MAX_ELEMENT(JoystickHat, Left);

    enum class GamepadButton : unsigned char {
        ButtonA                 = 0,
        ButtonB                 = 1,
        ButtonX                 = 2,
        ButtonY                 = 3,
        ButtonLeftBumper        = 4,
        ButtonRightBumper       = 5,
        ButtonBack              = 6,
        ButtonStart             = 7,
        ButtonGuide             = 8,
        ButtonLeftThumb         = 9,
        ButtonRightThumb        = 10,
        ButtonDPadUp            = 11,
        ButtonDPadRight         = 12,
        ButtonDPadDown          = 13,
        ButtonDPadLeft          = 14,

        ButtonCross             = ButtonA,
        ButtonCircle            = ButtonB,
        ButtonSquare            = ButtonX,
        ButtonTriangle          = ButtonY
    };

    enum class GamepadAxis : unsigned char {
        LeftX                   = 0,
        LeftY                   = 1,
        RightX                  = 2,
        RightY                  = 3,
        LeftTrigger             = 4,
        RightTrigger            = 5
    };
}
