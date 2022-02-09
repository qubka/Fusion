#include "KeyInput.hpp"
#include "Fusion/Core/Window.hpp"

using namespace Fusion;

std::vector<KeyInput*> KeyInput::instances;

KeyInput::KeyInput(const std::vector<int>& keysToMonitor) : Input(keysToMonitor) {
    instances.push_back(this);
}

KeyInput::~KeyInput() {
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void KeyInput::Setup(Window& window) {
    glfwSetKeyCallback(window, KeyInput::KeyCallback);
}

void KeyInput::Update() {
    /*for (auto* input : instances) {
        input->onUpdate();
    }*/
}

void KeyInput::KeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods) {
    for (auto* input : instances) {
        input->setKey(key, action);
    }
}