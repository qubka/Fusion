#include "joystick.hpp"
#include "device_manager.hpp"

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

Joystick::~Joystick() {
}

void Joystick::update() {
    if (!connected)
        return;

    int axeCount;
    auto axesPtr = glfwGetJoystickAxes(port, &axeCount);
    axes.resize(axeCount);

    for (int i = 0; i < axeCount; i++) {
        float ptr = axesPtr[i];
        if (axes[i] != ptr) {
            axes[i] = ptr;
            onAxis.publish(fe::JoyAxisData{ static_cast<fe::JoystickButton>(i), ptr });
        }
    }

    int buttonCount;
    auto buttonsPtr = glfwGetJoystickButtons(port, &buttonCount);
    buttons.resize(buttonCount);

    for (int i = 0; i < buttonCount; i++) {
        fe::ActionCode ptr = buttonsPtr[i];
        if (ptr != fe::Action::Release && buttons[i] != fe::Action::Release) {
            buttons[i] = fe::Action::Repeat;
        } else if (buttons[i] != ptr) {
            buttons[i] = ptr;
            onButton.publish(fe::JoyButtonData{ static_cast<fe::JoystickButton>(i), ptr });
        }
    }

    int hatCount;
    auto hatsPtr = glfwGetJoystickHats(port, &hatCount);
    hats.resize(hatCount);

    for (int i = 0; i < hatCount; i++) {
        fe::JoystickHat ptr = hatsPtr[i];
        if (hats[i] != ptr) {
            hats[i] = ptr;
            onHat.publish(fe::JoyHatData{ static_cast<fe::JoystickHat>(i), ptr });
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