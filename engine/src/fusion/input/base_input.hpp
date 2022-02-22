#pragma once

#include "mouse_codes.hpp"
#include "key_codes.hpp"
#include "action_codes.hpp"

namespace fe {
    /* Template class */

    /// Regards to Jonathan Heard
    /// @link https://stackoverflow.com/questions/55573238/how-do-i-do-a-proper-input-class-in-glfw-for-a-game-engine
    template<class T>
    class BaseInput {
    public:
        //! Takes a list of which keys to keep state for
        BaseInput(const std::initializer_list<T>& keysToMonitor);
        ~BaseInput() = default;

        virtual void onAttach() {}
        virtual void onUpdate() { currentFrame++; }
        virtual void onDetach() {}

        //! If disabled, Input.isKey_ always returns false
        bool isEnabled() const { return enabled; }
        void isEnabled(bool flag) { enabled = flag; }

    protected:
        bool isKey(T keycode) const;
        bool isKeyUp(T keycode) const;
        bool isKeyDown(T keycode) const;

        //! Used internally to update key states. Should be called by the GLFW callbacks
        void setKey(T keycode, ActionCode action);

        struct Key {
            bool pressed{ false };
            uint32_t lastFrame{ UINT32_MAX };
        };

        std::map<T, Key> keys;
        uint32_t currentFrame{ 0 };
        bool enabled{ true };
    };

    template class BaseInput<MouseCode>;
    template class BaseInput<KeyCode>;
}