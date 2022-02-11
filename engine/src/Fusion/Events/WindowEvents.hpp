#pragma once

#include "Event.hpp"

namespace Fusion {

    struct FUSION_API WindowResizeEvent : public Event {
    };

    struct FUSION_API WindowCloseEvent : public Event {
    };

    struct FUSION_API WindowTypedTypedEvent : public Event {
    };
}