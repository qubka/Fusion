#include "KeyInput.hpp"
#include "../Window.hpp"

using namespace Fusion;

std::vector<KeyInput*> KeyInput::instances;

KeyInput::KeyInput(const std::vector<int>& keysToMonitor) : Input(keysToMonitor) {
    instances.push_back(this);
}

KeyInput::~KeyInput() {
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void KeyInput::setupKeyInputs(Window& window) {
    auto* pWindow = static_cast<GLFWwindow *>(window.getNativeWindow());
    glfwSetKeyCallback(pWindow, KeyInput::keyCallback);
}

void KeyInput::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Send key event to all KeyInput instances
    for (auto* input : instances) {
        input->setKey(key, action);
    }
}