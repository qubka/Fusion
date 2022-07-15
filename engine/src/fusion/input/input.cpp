#include "input.hpp"

#include "fusion/core/time.hpp"
#include "fusion/utils/glm_extention.hpp"
#include "fusion/devices/device_manager.hpp"

#define MOUSE_BUTTONS 1024

using namespace fe;

Input::Input() {
    setupEvents(true);
}

Input::~Input() {
    setupEvents(false);
}

void Input::onUpdate() {
    delta = vec3::zero;
    scroll = vec3::zero;
}

bool Input::getKey(Key key) {
    auto keyId = static_cast<uint16_t>(key);
    return keys[keyId].pressed;
}

bool Input::getKeyDown(Key key) {
    auto keyId = static_cast<uint16_t>(key);
    auto& [press, frame] = keys[keyId];
    return press && frame == Time::FrameCount();
}

bool Input::getKeyUp(Key key) {
    auto keyId = static_cast<uint16_t>(key);
    auto& [press, frame] = keys[keyId];
    return !press && frame == Time::FrameCount();
}

bool Input::getMouseButton(MouseButton button) {
    auto buttonId = MOUSE_BUTTONS + static_cast<uint8_t>(button);
    return keys[buttonId].pressed;
}

bool Input::getMouseButtonDown(MouseButton button) {
    auto buttonId = MOUSE_BUTTONS + static_cast<uint8_t>(button);
    auto& [press, frame] = keys[buttonId];
    return press && frame == Time::FrameCount();
}

bool Input::getMouseButtonUp(MouseButton button) {
    auto buttonId = MOUSE_BUTTONS + static_cast<uint8_t>(button);
    auto& [press, frame] = keys[buttonId];
    return !press && frame == Time::FrameCount();
}

void Input::setupEvents(bool connect) {
    auto window = DeviceManager::Get()->getWindow(0);
    if (connect) {
        // Set the window events callbacks.
        window->OnMouseButton().connect<&Input::onMouseButton>(this);
        window->OnMouseMotion().connect<&Input::onMouseMotion>(this);
        window->OnMouseScroll().connect<&Input::onMouseScroll>(this);
        window->OnKey().connect<&Input::onKeyPress>(this);
    } else {
        // Remove the window events callbacks.
        window->OnMouseButton().disconnect<&Input::onMouseButton>(this);
        window->OnMouseMotion().disconnect<&Input::onMouseMotion>(this);
        window->OnMouseScroll().disconnect<&Input::onMouseScroll>(this);
        window->OnKey().disconnect<&Input::onKeyPress>(this);
    }
}

void Input::onMouseMotion(const glm::vec2& pos) {
    delta += pos - position;
    position = pos;
}

void Input::onMouseScroll(const glm::vec2& offset) {
    scroll = offset;
}

void Input::onMouseButton(MouseButton button, InputAction action, bitmask::bitmask<InputMod> mods) {
    if (action == InputAction::Repeat)
        return;

    auto buttonId = MOUSE_BUTTONS + static_cast<uint8_t>(button);
    auto& [press, frame] = keys[buttonId];
    press = action == InputAction::Press;
    frame = Time::FrameCount();
}

void Input::onKeyPress(Key key, InputAction action, Key scan, bitmask::bitmask<InputMod> mods) {
    if (action == InputAction::Repeat)
        return;

    auto keyId = static_cast<uint16_t>(key);
    auto& [press, frame] = keys[keyId];
    press = action == InputAction::Press;
    frame = Time::FrameCount();
}
