#pragma once

#include "base_input.hpp"
#include "mouse_codes.hpp"

namespace Fusion {
    class MouseInput : public BaseInput<MouseCode> {
    public:
        MouseInput(const std::initializer_list<MouseCode>& buttonsToMonitor)
            : BaseInput<MouseCode>{buttonsToMonitor} {};
        ~MouseInput() = default;

        void onUpdate() override {
            delta = vec2::zero;
            scroll = vec2::zero;
            BaseInput::onUpdate();
        };

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

        //! Used internally to update mouse data. Should be called by the GLFW callbacks
        void onMouseMoved(const glm::vec2& pos) { setCursorPosition(pos); };
        void onMouseScroll(const glm::vec2& offset) { setScrollOffset(offset); };
        void onMouseButton(MouseCode button, ActionCode action) { setKey(button, action); };

    private:
        glm::vec2 delta{};
        glm::vec2 position{};
        glm::vec2 scroll{};

        void setCursorPosition(const glm::vec2& pos) {
            delta += pos - position;
            position = pos;
        };
        void setScrollOffset(const glm::vec2& offset) {
            scroll = offset;
        };
    };
}
