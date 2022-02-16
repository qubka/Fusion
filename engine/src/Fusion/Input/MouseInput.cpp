#include "MouseInput.hpp"

#include "Fusion/Core/Window.hpp"
#include "Fusion/Events/MouseEvents.hpp"

using namespace Fusion;

std::vector<MouseInput*> MouseInput::instances;

MouseInput::MouseInput(const std::vector<MouseCode>& buttonsToMonitor) : BaseInput<MouseCode>{buttonsToMonitor} {
    instances.push_back(this);
}

MouseInput::~MouseInput() {
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void MouseInput::onUpdate() {
    delta = {};
    scroll = {};
    BaseInput::onUpdate();
}

void MouseInput::setCursorPosition(const glm::vec2& pos) {
    delta += pos - position;
    position = pos;
}

void MouseInput::setScrollOffset(const glm::vec2& offset) {
    scroll = offset;
}

void MouseInput::Setup(Window& window) {
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    glfwSetCursorEnterCallback(window, CursorEnterCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}

void MouseInput::CursorPositionCallback(GLFWwindow* handle, double mouseX, double mouseY) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.getEventQueue().submit(new MouseMovedEvent{{},{mouseX, mouseY}});

    // Polling system
    for (auto* input : instances) {
        input->setCursorPosition({mouseX, mouseY});
    }
}

void MouseInput::CursorEnterCallback(GLFWwindow* handle, int entered) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    if (entered)
        window.getEventQueue().submit(new MouseCursorEnterEvent{});
    else
        window.getEventQueue().submit(new MouseCursorLeftEvent{});
}

void MouseInput::ScrollCallback(GLFWwindow* handle, double offsetX, double offsetY) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));
    window.getEventQueue().submit(new MouseScrollEvent{{},{offsetX, offsetY}});

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
            window.getEventQueue().submit(new MouseButtonPressedEvent{{{}, static_cast<MouseCode>(button)}});
            break;
        case GLFW_RELEASE:
            window.getEventQueue().submit(new MouseButtonReleasedEvent{{{}, static_cast<MouseCode>(button)}});
            break;
    }

    // Polling system
    for (auto* input : instances) {
        input->setKey(button, action);
    }
}
