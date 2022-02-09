#pragma once

#include "Input.hpp"

namespace Fusion {
    class Window;

    class FUSION_API KeyInput : public Input {
    public:
        KeyInput(const std::vector<int>& keysToMonitor);
        ~KeyInput() override;

        //! Returns true while the user holds down the key identified by GLFW code.
        bool getKey(int key) const { return isKey(key); };
        //! Returns true during the frame the user releases the key identified by GLFW code.
        bool getKeyUp(int key) const { return isKeyUp(key); };
        //! Returns true during the frame the user starts pressing down the key identified by GLFW code.
        bool getKeyDown(int key) const { return isKeyDown(key); };

        //! Must be called before any KeyInput instances will work
        static void Setup(Window& window);
        //! Can be called before poll events to reset some values
        static void Update();
    private:
        /// Workaround for C++ class using a c-style-callback
        static void KeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods);

        static std::vector<KeyInput*> instances;
    };
}
