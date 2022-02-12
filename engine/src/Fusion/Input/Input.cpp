#include "Input.hpp"
#include "Fusion/Core/Time.hpp"

using namespace Fusion;

template<class T>
Input<T>::Input(const std::vector<T>& keysToMonitor) {
    for (T keycode : keysToMonitor) {
        keys.emplace(keycode, Key{});
    }
}

template<class T>
bool Input<T>::isKey(T keycode) const {
    if (enabled) {
        if (auto it = keys.find(keycode); it != keys.end()) {
            const auto& key{ it->second };
            return key.pressed;
        }
    }
    return false;
}

template<class T>
bool Input<T>::isKeyUp(T keycode) const {
    if (enabled) {
        if (auto it = keys.find(keycode); it != keys.end()) {
            const auto& key{ it->second };
            return !key.pressed && key.lastFrame == Time::FrameCount();
        }
    }
    return false;
}

template<class T>
bool Input<T>::isKeyDown(T keycode) const {
    if (enabled) {
        if (auto it = keys.find(keycode); it != keys.end()) {
            const auto& key{ it->second };
            return key.pressed && key.lastFrame == Time::FrameCount();
        }
    }
    return false;
}

template<class T>
void Input<T>::setKey(T keycode, uint8_t action) {
    if (action == GLFW_REPEAT)
        return;

    if (auto it = keys.find(keycode); it != keys.end()) {
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