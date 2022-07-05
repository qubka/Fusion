#include "joystick.hpp"

using namespace glfw;

Joystick::Joystick(uint8_t port)
    : port{port}
    , name{isGamePad() ? glfwGetGamepadName(port) : glfwGetJoystickName(port)}
{
    // Validate whether the specified joystick is present.
    if (glfwJoystickPresent(port) == GLFW_FALSE)
        throw std::runtime_error("GLFW joystick failed to be identified");

    glfwSetJoystickUserPointer(port, this);
}

void Joystick::onUpdate() {
    if (!connected)
        return;

    int axeCount;
    auto axesPtr = glfwGetJoystickAxes(port, &axeCount);
    axes.resize(axeCount);

    for (int i = 0; i < axeCount; i++) {
        float value = axesPtr[i];
        if (axes[i] != value) {
            axes[i] = value;
            onAxis.publish(i, value);
        }
    }

    int buttonCount;
    auto buttonsPtr = glfwGetJoystickButtons(port, &buttonCount);
    buttons.resize(buttonCount);

    for (int i = 0; i < buttonCount; i++) {
        auto value = static_cast<fe::InputAction>(buttonsPtr[i]);
        if (value != fe::InputAction::Release && buttons[i] != fe::InputAction::Release) {
            buttons[i] = fe::InputAction::Repeat;
        } else if (buttons[i] != value) {
            buttons[i] = value;
            onButton.publish(i, value);
        }
    }

    int hatCount;
    auto hatsPtr = glfwGetJoystickHats(port, &hatCount);
    hats.resize(hatCount);

    for (int i = 0; i < hatCount; i++) {
        auto value = bitmask::bitmask<fe::JoystickHat>(static_cast<fe::JoystickHat>(hatsPtr[i]));
        if (hats[i] != value) {
            hats[i] = value;
            onHat.publish(i, value);
        }
    }
}

bool Joystick::isGamePad() const {
    return glfwJoystickIsGamepad(port);
}

const fe::GamepadState& Joystick::getGamepadState() const {
    GLFWgamepadstate* gamepadState;
    if (glfwGetGamepadState(port, gamepadState) == GLFW_TRUE) {
        return *reinterpret_cast<const fe::GamepadState*>(gamepadState);
    }
    throw std::runtime_error("GLFW joystick does not have gamepad mapping");
}