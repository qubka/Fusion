#pragma once

#include "EventQueue.hpp"

namespace Fusion {

    struct  WindowMovedEvent : public Event {
        glm::ivec2 pos;
    };

    struct  WindowSizeEvent : public Event {
        int width;
        int height;
    };

    struct  WindowCloseEvent : public Event {
    };

    struct  WindowRefreshEvent : public Event {
    };

    struct  WindowFocusedEvent : public Event {
    };

    struct  WindowUnfocusedEvent : public Event {
    };

    struct  WindowIconifiedEvent : public Event {
    };

    struct  WindowDeiconifiedEvent : public Event {
    };

    struct  WindowFramebufferSizeEvent : public Event {
        int width;
        int height;
    };

#if GLFW_VERSION_MINOR >= 1
    struct  WindowFileDropCallback : public Event {
        int count;
        const char** paths;

        const char* operator[](int index) const {
            FE_ASSERT(index < count && "invalid argument index");
            return paths[index];
        }
    };
#endif
#if GLFW_VERSION_MINOR >= 3
    struct  WindowMaximizedEvent : public Event {
    };
    struct  WindowUnmaximizedEvent : public Event {
    };
    struct  WindowContentScaleEvent : public Event {
        glm::vec2 scale;
    };
#endif
}