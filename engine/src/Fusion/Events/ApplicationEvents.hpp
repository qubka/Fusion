#pragma once

namespace Fusion {

    struct FUSION_API MonitorConnectedEvent : public Event {
        const GLFWmonitor* monitor{nullptr};
    };

    struct FUSION_API MonitorDisconnectedEvent : public Event {
        const GLFWmonitor* monitor{nullptr};
    };

    struct FUSION_API JoystickConnectedEvent : public Event {
        int joystickId;
    };

    struct FUSION_API JoystickDisconnectedEvent : public Event {
        int joystickId;
    };
}
