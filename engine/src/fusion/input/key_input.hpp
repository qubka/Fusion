#pragma once

#include "base_input.hpp"

namespace fe {
    class KeyInput : public BaseInput {
    public:
        KeyInput(const std::pair<KeyCode, KeyCode>& range) : BaseInput{} {
            std::vector<KeyCode> keysToMonitor(range.second - range.first + 1);
            std::iota(keysToMonitor.begin(), keysToMonitor.end(), range.first);
            for (KeyCode keycode : keysToMonitor) {
                keys.emplace(keycode, KeyState{});
            }
        }
        KeyInput(const std::initializer_list<KeyCode>& keysToMonitor) : BaseInput{} {
            for (KeyCode keycode : keysToMonitor) {
                keys.emplace(keycode, KeyState{});
            }
        }

        void onUpdate() override {}

        //! Returns true while the user holds down the key identified by GLFW code.
        bool getKey(KeyCode key) const { return IsKey<KeyCode>(keys, key); }
        //! Returns true during the frame the user releases the key identified by GLFW code.
        bool getKeyUp(KeyCode key) const { return IsKeyUp<KeyCode>(keys, key); }
        //! Returns true during the frame the user starts pressing down the key identified by GLFW code.
        bool getKeyDown(KeyCode key) const { return IsKeyDown<KeyCode>(keys, key); }

        //! Used internally to update mouse data. Should be called by the GLFW callbacks

        void setKey(KeyData data) {
            SetKey<KeyCode>(keys, data.key, data.action);
        }

    private:
        std::map<KeyCode, KeyState> keys;
    };
}
