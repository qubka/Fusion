#pragma once

#include "../../Fusion.hpp"

namespace Fusion {

#define TOTAL_BUTTONS 1032
#define MOUSE_BUTTONS 1024

    class FUSION_API SystemInput : public Input {
    public:
        SystemInput(Window& window);
        ~SystemInput() override;

        void onUpdate() override;

        bool getKey(int keycode) override;
        bool getKeyDown(int keycode) override;

        bool getMouseButton(int button) override;
        bool getMouseButtonDown(int button) override;

        const glm::vec2& mousePosition() override;
        const glm::vec2& mouseDelta() override;

    private:
        std::array<bool, TOTAL_BUTTONS> keys{};
        std::array<uint32_t, TOTAL_BUTTONS> frames{};
        uint32_t current{};
        glm::vec2 delta{};
        glm::vec2 position{};

        void setKey(int key, int action);
        void setMouseButton(int button, int action);
        void setCursorPosition(const glm::vec2& pos);

        // Workaround for C++ class using a c-style-callback

        static void cursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

        static std::vector<SystemInput*> instances;
    };
}