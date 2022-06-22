#pragma once

#include "base_input.hpp"

namespace fe {
    class MouseInput : public BaseInput {
    public:
        MouseInput(const std::pair<MouseButton, MouseButton>& range) : BaseInput{} {
            std::vector<MouseButton> buttonsToMonitor(range.second - range.first + 1);
            std::iota(buttonsToMonitor.begin(), buttonsToMonitor.end(), range.first);
            for (MouseButton button : buttonsToMonitor) {
                buttons.emplace(button, KeyState{});
            }
        }

        MouseInput(const std::initializer_list<MouseButton>& buttonsToMonitor) : BaseInput{} {
            for (MouseButton button : buttonsToMonitor) {
                buttons.emplace(button, KeyState{});
            }
        }

        void onUpdate() override {
            delta = {};
            scroll = {};
        }

        //! Returns whether the given mouse button is held down.
        bool getMouseButton(MouseButton button) const { return IsKey<MouseButton>(buttons, button); }
        //!	Returns true during the frame the user releases the given mouse button.
        bool getMouseButtonUp(MouseButton button) const { return IsKeyUp<MouseButton>(buttons, button); }
        //! Returns true during the frame the user pressed the given mouse button.
        bool getMouseButtonDown(MouseButton button) const { return IsKeyDown<MouseButton>(buttons, button); }

        //! The current mouse position in pixel coordinates.
        const glm::vec2& mousePosition() const { return position; }
        //! The current mouse position in normalized screen coordinates [0, 1].
        const glm::vec2& mouseNormalizedPosition() const { return normalized; }
        //! The current mouse position delta.
        const glm::vec2& mousePositionDelta() const { return delta; }
        //! The current mouse scroll delta.
        const glm::vec2& mouseScroll() const { return scroll; }

        //! Used internally to update mouse data. Should be called by the GLFW callbacks

        void setMouseButton(MouseData data) {
            SetKey(buttons, data.button, data.action);
        }

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

    private:
        std::map<MouseButton, KeyState> buttons;
        glm::vec2 position{};
        glm::vec2 normalized{};
        glm::vec2 delta{};
        glm::vec2 scroll{};
    };
}
