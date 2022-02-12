#include "MouseInput.hpp"

#include "Fusion/Renderer/Window.hpp"
#include "Fusion/Events/MouseEvents.hpp"

using namespace Fusion;

std::vector<MouseInput*> MouseInput::instances;

MouseInput::MouseInput(const std::vector<MouseCode>& buttonsToMonitor) : Input<MouseCode>{buttonsToMonitor} {
    instances.push_back(this);
}

MouseInput::~MouseInput() {
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void MouseInput::setCursorPosition(const glm::vec2& pos) {
    delta = pos - position;
    position = pos;
}

void MouseInput::setScrollOffset(const glm::vec2& offset) {
    scroll = offset;
}

void MouseInput::Setup(Window& window) {
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);
}

void MouseInput::CursorPositionCallback(GLFWwindow* handle, double mouseX, double mouseY) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.bus().publish(new MouseMovedEvent{{}, {mouseX, mouseY}});

    // Polling system
    for (auto* input : instances) {
        input->setCursorPosition({mouseX, mouseY});
    }
}

void MouseInput::MouseScrollCallback(GLFWwindow* handle, double offsetX, double offsetY) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.bus().publish(new MouseScrollEvent{{}, {offsetX, offsetY}});

    // Polling system
    for (auto* input : instances) {
        input->setScrollOffset({offsetX, offsetY});
    }
}

void MouseInput::MouseButtonCallback(GLFWwindow* handle, int button, int action, int mode) {
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
    for (auto* input : instances) {
        input->setKey(button, action);
    }
}