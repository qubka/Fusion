#pragma once

#include "codes.h"

namespace fe {
    // if you want use individual inputs, not called init here, use BaseInput's classes
    class FUSION_API Input : public Module::Registrar<Input>{
    public:
        Input() = default;
        ~Input() override = default;

        //! Returns true while the user holds down the key identified by GLFW code.
        bool getKey(Key key);
        //! Returns true during the frame the user releases the key identified by GLFW code.
        bool getKeyDown(Key key);
        //! Returns true during the frame the user starts pressing down the key identified by GLFW code.
        bool getKeyUp(Key key);

        //! Returns whether the given mouse button is held down.
        bool getMouseButton(MouseButton button);
        //!	Returns true during the frame the user releases the given mouse button.
        bool getMouseButtonDown(MouseButton button);
        //! Returns true during the frame the user pressed the given mouse button.
        bool getMouseButtonUp(MouseButton button);

        //! The current mouse position in pixel coordinates.
        const glm::vec2& getMousePosition() const { return mousePosition; }
        //! The current mouse position delta.
        const glm::vec2& getMousePositionDelta() const { return mousePositionDelta; }
        //! The current mouse scroll.
        const glm::vec2& getMouseScroll() const { return mouseScroll; }

    private:
        void onStart() override;
        void onUpdate() override;

        void onMouseButton(MouseButton button, InputAction action, bitmask::bitmask<InputMod> mods);
        void onMouseMotion(const glm::vec2& pos);
        void onMouseScroll(const glm::vec2& offset);
        void onKeyPress(Key key, InputAction action, Key scan, bitmask::bitmask<InputMod> mods);

    private:
        struct KeyData {
            bool pressed{ false };
            uint64_t frame{ UINT64_MAX };
        };

        std::array<KeyData, 1032> keys{};
        glm::vec2 mousePosition{ 0.0f };
        glm::vec2 mousePositionDelta{ 0.0f };
        glm::vec2 mouseScroll{ 0.0f };
    };
}
