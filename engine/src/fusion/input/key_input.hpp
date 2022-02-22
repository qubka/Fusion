#pragma once

#include "base_input.hpp"
#include "key_codes.hpp"

namespace fe {
    class KeyInput : public BaseInput<KeyCode> {
    public:
        KeyInput(const std::initializer_list<KeyCode>& keysToMonitor)
            : BaseInput<KeyCode>{keysToMonitor} {};
        ~KeyInput() = default;

        //! Returns true while the user holds down the key identified by GLFW code.
        bool getKey(KeyCode key) const { return isKey(key); }
        //! Returns true during the frame the user releases the key identified by GLFW code.
        bool getKeyUp(KeyCode key) const { return isKeyUp(key); }
        //! Returns true during the frame the user starts pressing down the key identified by GLFW code.
        bool getKeyDown(KeyCode key) const { return isKeyDown(key); }

        //! Used internally to update mouse data. Should be called by the GLFW callbacks
        void onKeyPressed(KeyCode key, ActionCode action) { setKey(key, action); };
    };
}
