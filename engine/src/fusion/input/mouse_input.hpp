#pragma once

#include "base_input.hpp"
#include "mouse_codes.hpp"

class GLFWwindow;

namespace Fusion {
    class Window;

    class MouseInput : public BaseInput<MouseCode> {
    public:
        MouseInput(const std::vector<MouseCode>& buttonsToMonitor);
        ~MouseInput();

        void onUpdate() override;

        //! Returns whether the given mouse button is held down.
        bool getMouseButton(MouseCode button) const { return isKey(button); }
        //!	Returns true during the frame the user releases the given mouse button.
        bool getMouseButtonUp(MouseCode button) const { return isKeyUp(button); }
        //! Returns true during the frame the user pressed the given mouse button.
        bool getMouseButtonDown(MouseCode button) const { return isKeyDown(button); }

        //! The current mouse position in pixel coordinates.
        const glm::vec2& mousePosition() const { return position; }
        //! The current mouse position delta.
        const glm::vec2& mouseDelta() const { return delta; }
        //! The current mouse scroll delta.
        const glm::vec2& mouseScroll() const { return scroll; }

        //! Must be called before any MouseInput instances will work
        static void Update();

        /// Workaround for C++ class using a c-style-callback
        static void OnMouseMoved(const glm::vec2& pos);
        static void OnMouseScroll(const glm::vec2& offset);
        static void OnMouseButton(MouseCode button, ActionCode action);

    private:
        glm::vec2 delta{};
        glm::vec2 position{};
        glm::vec2 scroll{};

        //! Used internally to update mouse data. Should be called by the GLFW callbacks
        void setCursorPosition(const glm::vec2& pos);
        void setScrollOffset(const glm::vec2& offset);

        static std::vector<MouseInput*> instances;
    };
}
