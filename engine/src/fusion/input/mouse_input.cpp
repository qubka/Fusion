#include "mouse_input.hpp"

#include "fusion/renderer/glfw/window.hpp"
#include "fusion/events/mouse_events.hpp"

using namespace Fusion;

std::vector<MouseInput*> MouseInput::instances;

MouseInput::MouseInput(const std::vector<MouseCode>& buttonsToMonitor) : BaseInput<MouseCode>{buttonsToMonitor} {
    instances.push_back(this);
}

MouseInput::~MouseInput() {
    instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

void MouseInput::onUpdate() {
    delta = vec2::zero;
    scroll = vec2::zero;
    BaseInput::onUpdate();
}

void MouseInput::setCursorPosition(const glm::vec2& pos) {
    delta += pos - position;
    position = pos;
}

void MouseInput::setScrollOffset(const glm::vec2& offset) {
    scroll = offset;
}

void MouseInput::Update() {
    for (auto* input : instances) {
        input->onUpdate();
    }
}

void MouseInput::OnMouseMoved(const glm::vec2& pos) {
    for (auto* input : instances) {
        input->setCursorPosition(pos);
    }
}

void MouseInput::OnMouseScroll(const glm::vec2& offset) {
    for (auto* input : instances) {
        input->setScrollOffset(offset);
    }
}

void MouseInput::OnMouseButton(MouseCode button, ActionCode action) {
    for (auto* input : instances) {
        input->setKey(button, action);
    }
}