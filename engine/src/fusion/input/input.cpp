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

#if !defined(__ANDROID__)
void Input::CursorPosCallback(GLFWwindow* handle, double mouseX, double mouseY) {
    glm::vec2 mouse {mouseX, mouseY};

    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.getEventQueue().submit(new MouseMovedEvent{{}, mouse});

    delta += mouse - position;
    position = mouse;
}

void Input::CursorEnterCallback(GLFWwindow* handle, int entered) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    if (entered)
        window.getEventQueue().submit(new MouseCursorLeftEvent{});
    else
        window.getEventQueue().submit(new MouseCursorLeftEvent{});
}

void Input::ScrollCallback(GLFWwindow* handle, double offsetX, double offsetY) {
    glm::vec2 offset {offsetX, offsetY};

    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.getEventQueue().submit(new MouseScrollEvent{{}, offset});

    scroll = offset;
}

void Input::MouseButtonCallback(GLFWwindow* handle, int button, int action, int mode) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    // Event system
    switch (action) {
        case GLFW_PRESS:
            window.getEventQueue().submit(new MouseButtonPressedEvent{{{}, static_cast<MouseCode>(button)}});
            break;
        case GLFW_RELEASE:
            window.getEventQueue().submit(new MouseButtonReleasedEvent{{{}, static_cast<MouseCode>(button)}});
            break;
    }

    // Polling system
    int key = MOUSE_BUTTONS + button;
    switch (action) {
        case GLFW_PRESS:
            keys[key] = true;
            frames[key] = current;
            break;
        case GLFW_RELEASE:
            keys[key] = false;
            frames[key] = current;
            break;
        default: // GLFW_REPEAT
            break;
    }
}

void Input::KeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mode) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    // Event system
    switch (action) {
        case GLFW_PRESS:
            window.getEventQueue().submit(new KeyPressedEvent{{{}, static_cast<KeyCode>(key)}, false});
            break;
        case GLFW_RELEASE:
            window.getEventQueue().submit(new KeyReleasedEvent{{{}, static_cast<KeyCode>(key)}});
            break;
        case GLFW_REPEAT:
            window.getEventQueue().submit(new KeyPressedEvent{{{}, static_cast<KeyCode>(key)}, true});
            break;
    }

    // Polling system
    switch (action) {
        case GLFW_PRESS:
            keys[key] = true;
            frames[key] = current;
            break;
        case GLFW_RELEASE:
            keys[key] = false;
            frames[key] = current;
            break;
        default: // GLFW_REPEAT
            break;
    }
}

void Input::CharCallback(GLFWwindow* handle, unsigned int keycode) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.getEventQueue().submit(new KeyTypedEvent{{{}, static_cast<KeyCode>(keycode)}});
}
#else

#endif