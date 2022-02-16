#pragma once

#include "EventQueue.hpp"

namespace Fusion {

    struct FUSION_API WindowMovedEvent : public Event {
        glm::ivec2 pos;
    };

    struct FUSION_API WindowSizeEvent : public Event {
        int width;
        int height;
    };

    struct FUSION_API WindowCloseEvent : public Event {
    };

    struct FUSION_API WindowRefreshEvent : public Event {
    };

    struct FUSION_API WindowFocusedEvent : public Event {
    };

    struct FUSION_API WindowUnfocusedEvent : public Event {
    };

    struct FUSION_API WindowIconifiedEvent : public Event {
    };

    struct FUSION_API WindowDeiconifiedEvent : public Event {
    };

    struct FUSION_API WindowFramebufferSizeEvent : public Event {
        int width;
        int height;
    };

#if GLFW_VERSION_MINOR >= 1
    struct FUSION_API WindowFileDropCallback : public Event {
        int count;
        const char** paths;

        const char* operator[](int index) const {
            FE_ASSERT(index < count && "invalid argument index");
            return paths[index];
        }
    };
#endif
#if GLFW_VERSION_MINOR >= 3
    struct FUSION_API WindowMaximizedEvent : public Event {
    };
    struct FUSION_API WindowUnmaximizedEvent : public Event {
    };
    struct FUSION_API WindowContentScaleEvent : public Event {
        glm::vec2 scale;
    };
#endif
}