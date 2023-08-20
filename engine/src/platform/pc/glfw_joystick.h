#pragma once

#include "fusion/devices/joystick.h"

namespace fe::glfw {
    class FUSION_API Joystick final : public fe::Joystick {
        friend class DeviceManager;
    public:
        explicit Joystick(uint8_t port);
        ~Joystick() override = default;

        uint8_t getPort() const override { return port; }
        bool isConnected() const override { return connected; }
        bool isGamePad() const override;

        const std::string& getName() const override { return name; }

        size_t getButtonCount() const override { return connected ? buttons.size() : 0; }
        size_t getAxisCount() const override { return connected ? axes.size() : 0; }
        size_t getHatCount() const override { return connected ? hats.size() : 0; }

        InputAction getButton(uint8_t button) override { return connected && button < buttons.size() ? buttons[button] : InputAction::Release; }
        float getAxis(uint8_t axis) const override { return connected && axis < axes.size() ? axes[axis] : 0.0f; }
        bitmask::bitmask<JoystickHat> getHat(uint8_t hat) const override { return connected && hat < hats.size() ? hats[hat] : bitmask::bitmask<JoystickHat>(); }

        const GamepadState& getGamepadState() const override;

        void* getNativeJoystick() const override { return (void*) this; }

    protected:
        void onUpdate() override;

    private:
        uint8_t port;
        std::string name;
        std::vector<InputAction> buttons;
        std::vector<float> axes;
        std::vector<bitmask::bitmask<JoystickHat>> hats;
        bool connected{ true };
    };
}
