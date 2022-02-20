#include "input.hpp"

#include "fusion/core/window.hpp"
#include "fusion/events/key_events.hpp"
#include "fusion/events/mouse_events.hpp"

#define MOUSE_BUTTONS 500

using namespace Fusion;

bool Input::keys[]{};
uint32_t Input::frames[]{ UINT32_MAX };
uint32_t Input::current{};
glm::vec2 Input::delta{};
glm::vec2 Input::position{};
glm::vec2 Input::scroll{};

bool Input::GetKey(KeyCode keycode) {
    return keys[keycode];
}

bool Input::GetKeyDown(KeyCode keycode) {
    return keys[keycode] && frames[keycode] == current;
}

bool Input::GetKeyUp(KeyCode keycode) {
    return !keys[keycode] && frames[keycode] == current;
}

bool Input::GetMouseButton(MouseCode button) {
    int index = MOUSE_BUTTONS + button;
    return keys[index];
}

bool Input::GetMouseButtonDown(MouseCode button) {
    int index = MOUSE_BUTTONS + button;
    return keys[index] && frames[index] == current;
}

bool Input::GetMouseButtonUp(MouseCode button) {
    int index = MOUSE_BUTTONS + button;
    return !keys[index] && frames[index] == current;
}

void Input::Update() {
    delta = {};
    scroll = {};
    current++;
}

void Input::OnMouseMoved(const glm::vec2& pos) {
    delta += pos - position;
    position = pos;
}

void Input::OnMouseScroll(const glm::vec2& offset) {
    scroll = offset;
}

void Input::OnMouseButton(MouseCode button, ActionCode action) {
    // Polling system
    int key = MOUSE_BUTTONS + button;
    switch (action) {
        case Action::Press:
            keys[key] = true;
            frames[key] = current;
            break;
        case Action::Release:
            keys[key] = false;
            frames[key] = current;
            break;
        default: // Action::Repeat
            break;
    }
}

void Input::OnKeyPressed(KeyCode key, ActionCode action) {
    // Polling system
    switch (action) {
        case Action::Press:
            keys[key] = true;
            frames[key] = current;
            break;
        case Action::Release:
            keys[key] = false;
            frames[key] = current;
            break;
        default: // Action::Repeat
            break;
    }
}