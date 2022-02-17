#pragma once

#include "BaseInput.hpp"
#include "KeyCodes.hpp"

namespace Fusion {
    class Window;

    class FUSION_API KeyInput : public BaseInput<KeyCode> {
    public:
        KeyInput(const std::vector<KeyCode>& keysToMonitor);
        ~KeyInput();

        //! Returns true while the user holds down the key identified by GLFW code.
        bool getKey(KeyCode key) const { return isKey(key); }
        //! Returns true during the frame the user releases the key identified by GLFW code.
        bool getKeyUp(KeyCode key) const { return isKeyUp(key); }
        //! Returns true during the frame the user starts pressing down the key identified by GLFW code.
        bool getKeyDown(KeyCode key) const { return isKeyDown(key); }

        //! Must be called before any KeyInput instances will work
        static void Setup(Window& window);
    private:
        /// Workaround for C++ class using a c-style-callback
        static void KeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods);
        static void CharCallback(GLFWwindow* handle, unsigned int keycode);

        static std::vector<KeyInput*> instances;
    };
}
