#pragma once

#include "codes.hpp"

#include "fusion/core/module.hpp"

namespace fe {
    // if you want use individual inputs, not called init here, use BaseInput's classes
    class Input : public Module::Registrar<Input>{
    public:
        Input();
        ~Input() override;

        void onUpdate() override;

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
        const glm::vec2& getMousePosition() const { return position; }
        //! The current mouse position delta.
        const glm::vec2& getMouseDelta() const { return delta; }
        //! The current mouse scroll delta.
        const glm::vec2& getMouseScroll() const { return scroll; }

    private:
        void setupEvents(bool connect);

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
        glm::vec2 delta{ 0.0f };
        glm::vec2 position{ 0.0f };
        glm::vec2 scroll{ 0.0f };
    };
}
