#pragma once

#include "Event.hpp"

namespace Fusion {

    struct WindowResizeEvent : public Event {
        uint32_t width;
        uint32_t height;
    };

    struct WindowCloseEvent : public Event {
    };
}