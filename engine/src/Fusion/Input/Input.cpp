#include "Input.hpp"
#include "Fusion/Core/Time.hpp"

using namespace Fusion;

//std::map<std::type_index, Input*> Input::instances;

Input::Input(const std::vector<int>& keysToMonitor) {
    for (int keycode : keysToMonitor) {
        keys.emplace(keycode, Key{});
    }
}

Input::~Input() {

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
            return !key.pressed && key.lastFrame == Time::FrameCount();
        }
    }
    return false;
}

bool Input::isKeyDown(int keycode) const {
    if (enabled) {
        if (auto it{ keys.find(keycode) }; it != keys.end()) {
            const auto& key{ it->second };
            return key.pressed && key.lastFrame == Time::FrameCount();
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
                key.lastFrame = Time::FrameCount();
                break;
            case GLFW_RELEASE:
                key.pressed = false;
                key.lastFrame = Time::FrameCount();
                break;
            default: // GLFW_REPEAT
                break;
        }
    }
}
