#include "Input.hpp"

#include "Fusion/Events/KeyEvents.hpp"
#include "Fusion/Events/MouseEvents.hpp"

#define MOUSE_BUTTONS 1024

using namespace Fusion;

bool Input::keys[]{};
uint32_t Input::frames[]{std::numeric_limits<uint32_t>::max()};
uint32_t Input::current{};
glm::vec2 Input::delta{};
glm::vec2 Input::position{};
glm::vec2 Input::scroll{};

void Input::Init(Window& window) {
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    glfwSetCharCallback(window, CharCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);
}

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
    current++;
    delta.x = 0;
    delta.y = 0;
    scroll.x = 0;
    scroll.y = 0;
}

void Input::CursorPositionCallback(GLFWwindow* handle, double mouseX, double mouseY) {
    glm::vec2 mouse {mouseX, mouseY};

    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.bus().publish(new MouseMovedEvent{{}, mouse});

    delta += mouse - position;
    position = mouse;
}

void Input::MouseScrollCallback(GLFWwindow* handle, double offsetX, double offsetY) {
    glm::vec2 offset {offsetX, offsetY};

    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.bus().publish(new MouseScrollEvent{{}, offset});

    scroll = offset;
}

void Input::MouseButtonCallback(GLFWwindow* handle, int button, int action, int mode) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    // Event system
    switch (action) {
        case GLFW_PRESS:
            window.bus().publish(new MouseButtonPressedEvent{{{}, static_cast<MouseCode>(button)}});
            break;
        case GLFW_RELEASE:
            window.bus().publish(new MouseButtonReleasedEvent{{{}, static_cast<MouseCode>(button)}});
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
            window.bus().publish(new KeyPressedEvent{{{}, static_cast<KeyCode>(key)}, false});
            break;
        case GLFW_RELEASE:
            window.bus().publish(new KeyReleasedEvent{{{}, static_cast<KeyCode>(key)}});
            break;
        case GLFW_REPEAT:
            window.bus().publish(new KeyPressedEvent{{{}, static_cast<KeyCode>(key)}, true});
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

    window.bus().publish(new KeyTypedEvent{{{}, static_cast<KeyCode>(keycode)}});
}