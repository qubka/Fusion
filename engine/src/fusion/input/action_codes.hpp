#pragma once

namespace Fusion {
    using ActionCode = uint8_t;

    namespace Action {
        // From glfw3.h
        enum : ActionCode {
            Release              = 0, /* The key or mouse button was released. */
            Press                = 1, /* The key or mouse button was pressed. */
            Repeat               = 2, /* The key was held down until it repeated. */
        };
    }
}