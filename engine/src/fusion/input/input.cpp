#include "input.hpp"

#include "fusion/devices/device_manager.hpp"

#define MOUSE_BUTTONS 1024

using namespace fe;

void Input::onStart() {
    // Set the window events callbacks.
    auto window = DeviceManager::Get()->getWindow(0);
    window->OnMouseButton().connect<&Input::onMouseButton>(this);
    window->OnMouseMotion().connect<&Input::onMouseMotion>(this);
    window->OnMouseScroll().connect<&Input::onMouseScroll>(this);
    window->OnKey().connect<&Input::onKeyPress>(this);

    // TODO: Delete hooks ?
}

void Input::onUpdate() {
    mousePositionDelta = vec3::zero;
    mouseScroll = vec3::zero;
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

void Input::onMouseMotion(const glm::vec2& pos) {
    mousePositionDelta += pos - mousePosition;
    mousePosition = pos;
}

void Input::onMouseScroll(const glm::vec2& offset) {
    mouseScroll = offset;
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
