#pragma once

#include "Input.hpp"

namespace Fusion {
    class Window;

    class FUSION_API MouseInput : public Input {
    public:
        MouseInput(const std::vector<int>& keysToMonitor);
        ~MouseInput() override;

        bool getMouseButton(int button) const { return isKey(button); };
        bool getMouseButtonUp(int button) const { return isKeyUp(button); };
        bool getMouseButtonDown(int button) const { return isKeyDown(button); };

        const glm::vec2& mousePosition() const { return position; };
        const glm::vec2& mouseDelta() const { return delta; };

        static void setupMouseInputs(Window& window);
    private:
        glm::vec2 delta{};
        glm::vec2 position{};

        void setCursorPosition(const glm::vec2& pos);

        /// Workaround for C++ class using a c-style-callback

        static void cursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mode);

        static std::vector<MouseInput*> instances;
    };
}
