#pragma once

#include "base_input.hpp"
#include "mouse_codes.hpp"

namespace fe {
    class MouseInput : public BaseInput<MouseCode> {
    public:
        MouseInput()
            : BaseInput<MouseCode>{std::make_pair(Mouse::Button0, Mouse::ButtonLast)} {} //! use all possible buttonmaps
        MouseInput(const std::initializer_list<MouseCode>& buttonsToMonitor)
            : BaseInput<MouseCode>{buttonsToMonitor} {};
        ~MouseInput() = default;

        void onUpdate() override {
            delta = {};
            scroll = {};
            BaseInput::onUpdate();
        }

        //! Returns whether the given mouse button is held down.
        bool getMouseButton(MouseCode button) const { return isKey(button); }
        //!	Returns true during the frame the user releases the given mouse button.
        bool getMouseButtonUp(MouseCode button) const { return isKeyUp(button); }
        //! Returns true during the frame the user pressed the given mouse button.
        bool getMouseButtonDown(MouseCode button) const { return isKeyDown(button); }

        //! The current mouse position in pixel coordinates.
        const glm::vec2& mousePosition() const { return position; }
        //! The current mouse position in normalized screen coordinates [0, 1].
        const glm::vec2& mouseNormalizedPosition() const { return normalized; }
        //! The current mouse position delta.
        const glm::vec2& mouseDelta() const { return delta; }
        //! The current mouse scroll delta.
        const glm::vec2& mouseScroll() const { return scroll; }

    public:
        //! Used internally to update mouse data. Should be called by the GLFW callbacks
        void onMouseMotion(const glm::vec2& pos) { setMousePosition(pos); }
        void onMouseMotionNorm(const glm::vec2& pos) { setMousePositionNorm(pos); }
        void onMouseScroll(const glm::vec2& offset) { setScrollOffset(offset); }
        void onMouseButton(MouseData data) { setKey(data.button, data.action); };

    private:
        glm::vec2 position{};
        glm::vec2 normalized{};
        glm::vec2 delta{};
        glm::vec2 scroll{};

        void setMousePosition(const glm::vec2& pos) {
            delta += pos - position;
            position = pos;
        }

        void setMousePositionNorm(const glm::vec2& pos) {
            normalized = pos;
        }

        void setScrollOffset(const glm::vec2& offset) {
            scroll = offset;
        }
    };
}
