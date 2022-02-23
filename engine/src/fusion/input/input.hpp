#pragma once

#include "key_codes.hpp"
#include "mouse_codes.hpp"
#include "fusion/core/application.hpp"

namespace fe {
    class Input {
    public:
        static bool GetKey(KeyCode keycode) { return Application::Instance().getKeyInput().getKey(keycode); }
        static bool GetKeyDown(KeyCode keycode) { return Application::Instance().getKeyInput().getKeyDown(keycode); }
        static bool GetKeyUp(KeyCode keycode) { return Application::Instance().getKeyInput().getKeyUp(keycode); }

        static bool GetMouseButton(MouseCode button) { return Application::Instance().getMouseInput().getMouseButton(button); }
        static bool GetMouseButtonDown(MouseCode button) { return Application::Instance().getMouseInput().getMouseButtonDown(button); }
        static bool GetMouseButtonUp(MouseCode button) { return Application::Instance().getMouseInput().getMouseButtonUp(button); }

        static const glm::vec2& MousePosition() { return Application::Instance().getMouseInput().mousePosition(); }
        static const glm::vec2& MouseDelta() { return Application::Instance().getMouseInput().mouseDelta(); }
        static const glm::vec2& MouseScroll() { return Application::Instance().getMouseInput().mouseScroll(); }
    };
}
