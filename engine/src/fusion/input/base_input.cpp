#include "base_input.hpp"

using namespace Fusion;

template<class T>
BaseInput<T>::BaseInput(const std::initializer_list<T>& keysToMonitor) {
    for (T keycode : keysToMonitor) {
        keys.emplace(keycode, Key{});
    }
}

template<class T>
bool BaseInput<T>::isKey(T keycode) const {
    if (enabled) {
        if (auto it { keys.find(keycode) }; it != keys.end()) {
            const auto& key{ it->second };
            return key.pressed;
        }
    }
    return false;
}

template<class T>
bool BaseInput<T>::isKeyUp(T keycode) const {
    if (enabled) {
        if (auto it { keys.find(keycode) }; it != keys.end()) {
            const auto& key{ it->second };
            return !key.pressed && key.lastFrame == currentFrame;
        }
    }
    return false;
}

template<class T>
bool BaseInput<T>::isKeyDown(T keycode) const {
    if (enabled) {
        if (auto it { keys.find(keycode) }; it != keys.end()) {
            const auto& key{ it->second };
            return key.pressed && key.lastFrame == currentFrame;
        }
    }
    return false;
}

template<class T>
void BaseInput<T>::setKey(T keycode, ActionCode action) {
    if (action == Action::Repeat)
        return;

    if (auto it { keys.find(keycode) }; it != keys.end()) {
        auto& key{ it->second };
        switch (action) {
            case Action::Press:
                key.pressed = true;
                key.lastFrame = currentFrame;
                break;
            case Action::Release:
                key.pressed = false;
                key.lastFrame = currentFrame;
                break;
            default: // Repeat
                break;
        }
    }
}