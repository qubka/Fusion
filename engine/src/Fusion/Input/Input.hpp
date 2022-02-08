#pragma once

#include "Fusion/Core/Base.hpp"

namespace Fusion {
    class Window;

    /// Regards to Jonathan Heard
    /// @link https://stackoverflow.com/questions/55573238/how-do-i-do-a-proper-input-class-in-glfw-for-a-game-engine
    class FUSION_API Input {
    public:
        //! Takes a list of which keys to keep state for
        Input(const std::vector<int>& keysToMonitor);
        virtual ~Input() = default;

        //virtual void onUpdate() { currentFrame++; }
        //virtual void onAttach() {}
        //virtual void onDetach() {}

        //! If disabled, Input.isKey_ always returns false
        bool isEnabled() const { return enabled; }
        void isEnabled(bool flag) { enabled = flag; }
    protected:
        bool isKey(int keycode) const;
        bool isKeyUp(int keycode) const;
        bool isKeyDown(int keycode) const;

        //! Used internally to update key states. Should be called by the GLFW callbacks
        void setKey(int keycode, int action);

        struct Key {
            bool pressed{false};
            uint64_t frame{std::numeric_limits<uint64_t>::max()};
        };

        std::map<int, Key> keys;
        bool enabled{true};
    };
}