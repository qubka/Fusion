#include "MouseInput.hpp"
#include "Fusion/Window.hpp"

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

void MouseInput::SetupMouseInputs(Window& window) {
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    //glfwSetScrollCallback(window, scrollCallback);
}

void MouseInput::CursorPositionCallback(GLFWwindow* handle, double mouseX, double mouseY) {
    for (auto* input : instances) {
        input->setCursorPosition({mouseX, mouseY});
    }
}

void MouseInput::MouseButtonCallback(GLFWwindow* handle, int button, int action, int mode) {
    for (auto* input : instances) {
        input->setKey(button, action);
    }
}
