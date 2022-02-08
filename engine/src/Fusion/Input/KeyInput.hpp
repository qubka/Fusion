#pragma once

#include "Input.hpp"

namespace Fusion {
    class Window;

    class FUSION_API KeyInput : public Input {
    public:
        KeyInput(const std::vector<int>& keysToMonitor);
        ~KeyInput() override;

        bool getKey(int key) const { return isKey(key); };
        bool getKeyUp(int key) const { return isKeyUp(key); };
        bool getKeyDown(int key) const { return isKeyDown(key); };

        static void SetupKeyInputs(Window& window);
    private:
        /// Workaround for C++ class using a c-style-callback

        static void KeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods);

        static std::vector<KeyInput*> instances;
    };
}
