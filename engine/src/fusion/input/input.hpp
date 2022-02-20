#pragma once

#include "mouse_codes.hpp"
#include "key_codes.hpp"
#include "action_codes.hpp"

class GLFWwindow;

namespace Fusion {
    /* Global class (Testing purpose_ */

    // if you want use individual inputs, not called init here, use BaseInput's classes
    class Input {
    public:
        //! Returns true while the user holds down the key identified by GLFW code.
        static bool GetKey(KeyCode keycode);
        //! Returns true during the frame the user releases the key identified by GLFW code.
        static bool GetKeyDown(KeyCode keycode);
        //! Returns true during the frame the user starts pressing down the key identified by GLFW code.
        static bool GetKeyUp(KeyCode keycode);

        //! Returns whether the given mouse button is held down.
        static bool GetMouseButton(MouseCode button);
        //!	Returns true during the frame the user releases the given mouse button.
        static bool GetMouseButtonDown(MouseCode button);
        //! Returns true during the frame the user pressed the given mouse button.
        static bool GetMouseButtonUp(MouseCode button);

        //! The current mouse position in pixel coordinates.
        static glm::vec2& MousePosition() { return position; }
        //! The current mouse position delta.
        static glm::vec2& MouseDelta() { return delta; }
        //! The current mouse scroll delta.
        static glm::vec2& MouseScroll() { return scroll; }

        static void Update();

    private:
        static bool keys[512];
        static uint32_t frames[512];
        static uint32_t current;
        static glm::vec2 delta;
        static glm::vec2 position;
        static glm::vec2 scroll;

    public:
        /// Workaround for C++ class using a c-style-callback
        static void OnMouseMoved(const glm::vec2& pos);
        static void OnMouseScroll(const glm::vec2& offset);
        static void OnMouseButton(MouseCode button, ActionCode action);
        static void OnKeyPressed(KeyCode key, ActionCode action);
    };
}
