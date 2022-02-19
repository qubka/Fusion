#include "key_input.hpp"

#include "fusion/core/window.hpp"
#include "fusion/events/key_events.hpp"

using namespace Fusion;

std::vector<KeyInput*> KeyInput::instances;

KeyInput::KeyInput(const std::vector<KeyCode>& keysToMonitor) : BaseInput<KeyCode>{keysToMonitor} {
    instances.push_back(this);
}

KeyInput::~KeyInput() {
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void KeyInput::Setup(Window& window) {
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
}

void KeyInput::KeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods) {
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
    for (auto* input : instances) {
        input->setKey(key, action);
    }
}

void KeyInput::CharCallback(GLFWwindow* handle, unsigned int keycode) {
    auto& window = *static_cast<Window *>(glfwGetWindowUserPointer(handle));

    window.getEventQueue().submit(new KeyTypedEvent{{{}, static_cast<KeyCode>(keycode)}});
}