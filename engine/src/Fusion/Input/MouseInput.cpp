#include "MouseInput.hpp"
#include "../Window.hpp"

using namespace Fusion;

std::vector<MouseInput*> MouseInput::instances;

MouseInput::MouseInput(const std::vector<int>& keysToMonitor) : Input(keysToMonitor) {
    instances.push_back(this);
}

MouseInput::~MouseInput() {
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void MouseInput::setCursorPosition(const glm::vec2& pos) {
    delta += pos - position;
    position = pos;
}

void MouseInput::setupMouseInputs(Window& window) {
    auto* pWindow = static_cast<GLFWwindow *>(window.getNativeWindow());
    glfwSetMouseButtonCallback(pWindow, mouseButtonCallback);
    glfwSetCursorPosCallback(pWindow, cursorPositionCallback);
    //glfwSetScrollCallback(pWindow, scrollCallback);
}

void MouseInput::cursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY) {
    for (auto* input : instances) {
        input->setCursorPosition({mouseX, mouseY});
    }
}

void MouseInput::mouseButtonCallback(GLFWwindow* window, int button, int action, int mode) {
    for (auto* input : instances) {
        input->setKey(button, action);
    }
}
