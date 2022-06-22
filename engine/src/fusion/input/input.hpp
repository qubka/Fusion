#pragma once

#include "fusion/core/engine.hpp"

#include "key_codes.hpp"
#include "mouse_buttons.hpp"

namespace fe {
    class Input {
    public:
        static bool GetKey(KeyCode keycode) { return Engine::Get()->getApp()->getKeyInput().getKey(keycode); }
        static bool GetKeyDown(KeyCode keycode) { return Engine::Get()->getApp()->getKeyInput().getKeyDown(keycode); }
        static bool GetKeyUp(KeyCode keycode) { return Engine::Get()->getApp()->getKeyInput().getKeyUp(keycode); }

        static bool GetMouseButton(MouseButton button) { return Engine::Get()->getApp()->getMouseInput().getMouseButton(button); }
        static bool GetMouseButtonDown(MouseButton button) { return Engine::Get()->getApp()->getMouseInput().getMouseButtonDown(button); }
        static bool GetMouseButtonUp(MouseButton button) { return Engine::Get()->getApp()->getMouseInput().getMouseButtonUp(button); }

        static const glm::vec2& MousePosition() { return Engine::Get()->getApp()->getMouseInput().mousePosition(); }
        static const glm::vec2& MouseNormalizedPosition() { return Engine::Get()->getApp()->getMouseInput().mouseNormalizedPosition(); }
        static const glm::vec2& MousePositionDelta() { return Engine::Get()->getApp()->getMouseInput().mousePositionDelta(); }
        static const glm::vec2& MouseScroll() { return Engine::Get()->getApp()->getMouseInput().mouseScroll(); }
    };
}
