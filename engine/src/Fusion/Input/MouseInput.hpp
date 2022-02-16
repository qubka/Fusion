#pragma once

#include "BaseInput.hpp"
#include "MouseCodes.hpp"

namespace Fusion {
    class Window;

    class FUSION_API MouseInput : public BaseInput<MouseCode> {
    public:
        MouseInput(const std::vector<MouseCode>& buttonsToMonitor);
        ~MouseInput();

        void onUpdate() override;

        //! Returns whether the given mouse button is held down.
        bool getMouseButton(MouseCode button) const { return isKey(button); };
        //!	Returns true during the frame the user releases the given mouse button.
        bool getMouseButtonUp(MouseCode button) const { return isKeyUp(button); };
        //! Returns true during the frame the user pressed the given mouse button.
        bool getMouseButtonDown(MouseCode button) const { return isKeyDown(button); };

        //! The current mouse position in pixel coordinates.
        const glm::vec2& mousePosition() const { return position; };
        //! The current mouse position delta.
        const glm::vec2& mouseDelta() const { return delta; };
        //! The current mouse scroll delta.
        const glm::vec2& mouseScroll() const { return scroll; };

        //! Must be called before any MouseInput instances will work
        static void Setup(Window& window);
    private:
        glm::vec2 delta{};
        glm::vec2 position{};
        glm::vec2 scroll{};

        //! Used internally to update mouse data. Should be called by the GLFW callbacks
        void setCursorPosition(const glm::vec2& pos);
        void setScrollOffset(const glm::vec2& offset);

        /// Workaround for C++ class using a c-style-callback
        static void CursorPositionCallback(GLFWwindow* handle, double mouseX, double mouseY);
        static void CursorEnterCallback(GLFWwindow* handle, int entered);
        static void ScrollCallback(GLFWwindow* handle, double offsetX, double offsetY);
        static void MouseButtonCallback(GLFWwindow* handle, int button, int action, int mode);

        static std::vector<MouseInput*> instances;
    };
}
