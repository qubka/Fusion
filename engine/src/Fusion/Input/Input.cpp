#include "Input.hpp"

#define MOUSE_BUTTONS 1024

using namespace Fusion;

bool Input::keys[]{};
uint32_t Input::frames[]{std::numeric_limits<uint32_t>::max()};
uint32_t Input::current{};
glm::vec2 Input::delta{};
glm::vec2 Input::position{};

void Input::Init(Window& window) {
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    //glfwSetScrollCallback
}

bool Input::GetKey(int keycode) {
    if (keycode < 0 || keycode >= MOUSE_BUTTONS)
        return false;
    return keys[keycode];
}

bool Input::GetKeyDown(int keycode) {
    if (keycode < 0 || keycode >= MOUSE_BUTTONS)
        return false;
    return keys[keycode] && frames[keycode] == current;
}

bool Input::GetMouseButton(int button) {
    int index = MOUSE_BUTTONS + button;
    return keys[index];
}

bool Input::GetMouseButtonDown(int button) {
    int index = MOUSE_BUTTONS + button;
    return keys[index] && frames[index] == current;
}

void Input::Update() {
    current++;
    delta.x = 0;
    delta.y = 0;
}

glm::vec2& Input::MousePosition() {
    return position;
}

glm::vec2& Input::MouseDelta() {
    return delta;
}

void Input::CursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY) {
    glm::vec2 mouse {mouseX, mouseY};
    delta += mouse - position;
    position = mouse;
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mode) {
    int key = MOUSE_BUTTONS + button;
    Input::KeyCallback(window, key, 0, action, mode);
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
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