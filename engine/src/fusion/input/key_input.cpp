#include "key_input.hpp"

#include "fusion/renderer/glfw/window.hpp"
#include "fusion/events/key_events.hpp"

using namespace Fusion;

std::vector<KeyInput*> KeyInput::instances;

KeyInput::KeyInput(const std::vector<KeyCode>& keysToMonitor) : BaseInput<KeyCode>{keysToMonitor} {
    instances.push_back(this);
}

KeyInput::~KeyInput() {
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void KeyInput::Update() {
    for (auto* input : instances) {
        input->onUpdate();
    }
}

void KeyInput::OnKeyPressed(KeyCode key, ActionCode action) {
    for (auto* input : instances) {
        input->setKey(key, action);
    }
}