#pragma once

#include "Event.hpp"

namespace Fusion {

    struct WindowResizeEvent : public Event {
        int width;
        int height;
    };

    struct WindowCloseEvent : public Event {
    };
}