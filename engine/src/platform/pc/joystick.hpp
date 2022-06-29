#pragma once

#include "fusion/devices/joystick.hpp"

#include <GLFW/glfw3.h>

namespace glfw {
    class Joystick : public fe::Joystick {
    public:
        explicit Joystick(uint8_t port);
        ~Joystick() override = default;

        void update(float dt) override;

        uint8_t getPort() const override { return port; }
        bool isConnected() const override { return connected; }
        bool isGamePad() const override;

        const std::string& getName() const override { return name; }

        size_t getButtonCount() const override { return connected ? buttons.size() : 0; }
        size_t getAxisCount() const override { return connected ? axes.size() : 0; }
        size_t getHatCount() const override { return connected ? hats.size() : 0; }

        fe::ActionCode getButton(fe::JoystickButton button) override { return connected && button < buttons.size() ? buttons[button] : 0; }
        float getAxis(fe::JoystickAxis axis) const override { return connected && axis < axes.size() ? axes[axis] : 0.0f; }
        int getHat(fe::JoystickHat hat) const override { return connected && hat < hats.size() ? hats[hat] : 0; }

        const fe::GamepadState& getGamepadState() const override;

        void* getNativeJoystick() const override { return (void*) this; }

    private:
        uint8_t port;
        std::string name;
        std::vector<uint8_t> buttons;
        std::vector<float> axes;
        std::vector<fe::JoystickHat> hats;
        bool connected{ true };
    };
}
