#include "BaseInput.hpp"

using namespace Fusion;

template<class T>
BaseInput<T>::BaseInput(const std::vector<T>& keysToMonitor) {
    for (T keycode : keysToMonitor) {
        keys.emplace(keycode, Key{});
    }
}

template<class T>
bool BaseInput<T>::isKey(T keycode) const {
    if (enabled) {
        if (auto it = keys.find(keycode); it != keys.end()) {
            const auto& key{ it->second };
            return key.pressed;
        }
    }
    return false;
}

template<class T>
bool BaseInput<T>::isKeyUp(T keycode) const {
    if (enabled) {
        if (auto it = keys.find(keycode); it != keys.end()) {
            const auto& key{ it->second };
            return !key.pressed && key.lastFrame == currentFrame;
        }
    }
    return false;
}

template<class T>
bool BaseInput<T>::isKeyDown(T keycode) const {
    if (enabled) {
        if (auto it = keys.find(keycode); it != keys.end()) {
            const auto& key{ it->second };
            return key.pressed && key.lastFrame == currentFrame;
        }
    }
    return false;
}

template<class T>
void BaseInput<T>::setKey(T keycode, uint8_t action) {
    if (action == GLFW_REPEAT)
        return;

    if (auto it = keys.find(keycode); it != keys.end()) {
        auto& key{ it->second };
        switch (action) {
            case GLFW_PRESS:
                key.pressed = true;
                key.lastFrame = currentFrame;
                break;
            case GLFW_RELEASE:
                key.pressed = false;
                key.lastFrame = currentFrame;
                break;
            default: // GLFW_REPEAT
                break;
        }
    }
}