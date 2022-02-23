#pragma once

namespace fe {

    struct MonitorConnectedEvent : public Event {
        const GLFWmonitor* monitor{nullptr};
    };

    struct MonitorDisconnectedEvent : public Event {
        const GLFWmonitor* monitor{nullptr};
    };

    struct JoystickConnectedEvent : public Event {
        int joystickId;
    };

    struct JoystickDisconnectedEvent : public Event {
        int joystickId;
    };
}
