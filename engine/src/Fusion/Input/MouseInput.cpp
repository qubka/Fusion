#include "MouseInput.hpp"
#include "Fusion/Core/Window.hpp"

using namespace Fusion;

std::vector<MouseInput*> MouseInput::instances;

MouseInput::MouseInput(const std::vector<int>& buttonsToMonitor) : Input(buttonsToMonitor) {
    instances.push_back(this);
}

MouseInput::~MouseInput() {
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void MouseInput::setCursorPosition(const glm::vec2& pos) {
    delta += pos - position;
    position = pos;
}

void MouseInput::setScrollOffset(const glm::vec2& offset) {
    scroll = offset;
}

void MouseInput::onUpdate() {
    Input::onUpdate();
    delta.x = 0;
    delta.y = 0;
}

void MouseInput::Setup(Window& window) {
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);
}

void MouseInput::Update() {
    for (auto* input : instances) {
        input->onUpdate();
    }
}

void MouseInput::CursorPositionCallback(GLFWwindow* handle, double mouseX, double mouseY) {
    for (auto* input : instances) {
        input->setCursorPosition({mouseX, mouseY});
    }
}

void MouseInput::MouseScrollCallback(GLFWwindow* handle, double offsetX, double offsetY) {
    for (auto* input : instances) {
        input->setScrollOffset({offsetX, offsetY});
    }
}

void MouseInput::MouseButtonCallback(GLFWwindow* handle, int button, int action, int mode) {
    for (auto* input : instances) {
        input->setKey(button, action);
    }
}