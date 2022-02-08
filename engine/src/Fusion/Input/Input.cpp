#include "Input.hpp"
#include "Fusion/Core/Window.hpp"

using namespace Fusion;

Input::Input(const std::vector<int>& keysToMonitor) {
    for (int keycode : keysToMonitor) {
        keys.emplace(keycode, Key{});
    }
}

bool Input::isKey(int keycode) const {
    if (enabled) {
        if (auto it { keys.find(keycode) }; it != keys.end()) {
            const auto& key{ it->second };
            return key.pressed;
        }
    }
    return false;
}

bool Input::isKeyUp(int keycode) const {
    if (enabled) {
        if (auto it{ keys.find(keycode) }; it != keys.end()) {
            const auto& key{ it->second };
            return !key.pressed && key.frame == currentFrame;
        }
    }
    return false;
}

bool Input::isKeyDown(int keycode) const {
    if (enabled) {
        if (auto it{ keys.find(keycode)}; it != keys.end()) {
            const auto& key{ it->second };
            return key.pressed && key.frame == currentFrame;
        }
    }
    return false;
}

void Input::setKey(int keycode, int action) {
    if (action == GLFW_REPEAT)
        return;

    if (auto it{ keys.find(keycode)}; it != keys.end()) {
        auto& key{ it->second };
        switch (action) {
            case GLFW_PRESS:
                key.pressed = true;
                key.frame = currentFrame;
                break;
            case GLFW_RELEASE:
                key.pressed = false;
                key.frame = currentFrame;
                break;
            default: // GLFW_REPEAT
                break;
        }
    }
}