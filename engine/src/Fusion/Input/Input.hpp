#pragma once

#include "GLFW/glfw3.h"

#include "MouseCodes.hpp"
#include "KeyCodes.hpp"

namespace Fusion {
    class Window;

    /// Regards to Jonathan Heard
    /// @link https://stackoverflow.com/questions/55573238/how-do-i-do-a-proper-input-class-in-glfw-for-a-game-engine
    template<class T>
    class FUSION_API Input {
    public:
        //! Takes a list of which keys to keep state for
        Input(const std::vector<T>& keysToMonitor);
        ~Input() = default;

        /*virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onUpdate() { currentFrame++; }*/

        //! If disabled, Input.isKey_ always returns false
        bool isEnabled() const { return enabled; }
        void isEnabled(bool flag) { enabled = flag; }
    protected:
        bool isKey(T keycode) const;
        bool isKeyUp(T keycode) const;
        bool isKeyDown(T keycode) const;

        //! Used internally to update key states. Should be called by the GLFW callbacks
        void setKey(T keycode, uint8_t action);

        struct Key {
            bool pressed{false};
            uint64_t lastFrame{std::numeric_limits<uint64_t>::max()};
        };

        std::map<T, Key> keys;
        //uint32_t currentFrame{0}; // can be used instead of global if needed
        bool enabled{true};
    };

    template class Input<MouseCode>;
    template class Input<KeyCode>;
}