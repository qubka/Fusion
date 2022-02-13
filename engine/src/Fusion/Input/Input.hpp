#pragma once

#include "MouseCodes.hpp"
#include "KeyCodes.hpp"

#include "Fusion/Renderer/Window.hpp"

namespace Fusion {
    /* Global class */

    class FUSION_API Input {
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
        static glm::vec2& MousePosition() { return position; };
        //! The current mouse position delta.
        static glm::vec2& MouseDelta() { return delta; };
        //! The current mouse scroll delta.
        static glm::vec2& MouseScroll() { return scroll; };

    private:
        static bool keys[1032];
        static uint32_t frames[1032];
        static uint32_t current;
        static glm::vec2 delta;
        static glm::vec2 position;
        static glm::vec2 scroll;

        static void Init(Window& window);
        static void Update();

        friend class Application;

    protected:
        static void CursorPositionCallback(GLFWwindow* handle, double mouseX, double mouseY);
        static void MouseScrollCallback(GLFWwindow* handle, double offsetX, double offsetY);
        static void MouseButtonCallback(GLFWwindow* handle, int button, int action, int mode);
        static void KeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mode);
        static void CharCallback(GLFWwindow* handle, unsigned int keycode);
    };
}
