#pragma once

#include "MouseCodes.hpp"
#include "KeyCodes.hpp"

#include "Fusion/Renderer/Window.hpp"

namespace Fusion {
    /* Global class */

    class FUSION_API Input {
    public:
        static bool GetKey(int keycode);
        static bool GetKeyDown(int keycode);

        static bool GetMouseButton(int button);
        static bool GetMouseButtonDown(int button);

        static glm::vec2& MousePosition();
        static glm::vec2& MouseDelta();

    private:
        static bool keys[1032];
        static uint32_t frames[1032];
        static uint32_t current;
        static glm::vec2 delta;
        static glm::vec2 position;

        static void Init(Window& window);
        static void Update();

        friend class Application;

    protected:
        static void CursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
    };
}
