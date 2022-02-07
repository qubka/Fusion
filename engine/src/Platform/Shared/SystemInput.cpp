#include "SystemInput.hpp"

using namespace Fusion;

std::vector<SystemInput*> SystemInput::instances;

Input* Input::create(Window& window) {
    return new SystemInput(window);
}

SystemInput::SystemInput(Window& window) : Input() {
    auto* pWindow = reinterpret_cast<GLFWwindow *>(window.getNativeWindow());
    glfwSetKeyCallback(pWindow, keyCallback);
    glfwSetMouseButtonCallback(pWindow, mouseButtonCallback);
    glfwSetCursorPosCallback(pWindow, cursorPositionCallback);
    instances.push_back(this);
}

SystemInput::~SystemInput() {
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void SystemInput::onUpdate() {
    current++;
    delta.x = 0;
    delta.y = 0;
}

bool SystemInput::getKey(int keycode) {
    if (keycode < 0 || keycode >= MOUSE_BUTTONS)
        return false;
    return keys[keycode];
}

bool SystemInput::getKeyDown(int keycode) {
    if (keycode < 0 || keycode >= MOUSE_BUTTONS)
        return false;
    return keys[keycode] && frames[keycode] == current;
}

bool SystemInput::getMouseButton(int button) {
    int index = MOUSE_BUTTONS + button;
    return keys[index];
}

bool SystemInput::getMouseButtonDown(int button) {
    int index = MOUSE_BUTTONS + button;
    return keys[index] && frames[index] == current;
}

void SystemInput::setCursorPosition(const glm::vec2& pos) {
    delta += pos - position;
    position = pos;
}

void SystemInput::setKey(int key, int action) {
    switch (action) {
        case GLFW_PRESS:
        case GLFW_REPEAT:
            keys[key] = true;
            frames[key] = current;
            break;
        case GLFW_RELEASE:
            keys[key] = false;
            frames[key] = current;
            break;
    }
}

void SystemInput::setMouseButton(int button, int action) {
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
    }
}

const glm::vec2& SystemInput::mousePosition() {
    return position;
}

const glm::vec2& SystemInput::mouseDelta() {
    return delta;
}

/* Static callbacks */

void SystemInput::cursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY) {
    for (auto* input : instances) {
        input->setCursorPosition({mouseX, mouseY});
    }
}

void SystemInput::mouseButtonCallback(GLFWwindow* window, int button, int action, int mode) {
    for (auto* input : instances) {
        input->setMouseButton(button, action);
    }
}

void SystemInput::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    for (auto* input : instances) {
        input->setKey(key, action);
    }
}