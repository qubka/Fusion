#pragma once

namespace fe {
    using JoyState = uint8_t;

    namespace Key {
        // From glfw3.h
        enum : JoyState {
            HatCentered             = 0,
            HatUp                   = 1,
            HatRight                = 2,
            HatDown                 = 4,
            HatLeft                 = 8,
            HatRightUp              = HatRight | HatUp,
            HatRightDown            = HatRight | HatDown,
            HatLeftUp               = HatLeft | HatUp,
            HatLeftDown             = HatLeft | HatDown,
        };
    }
}