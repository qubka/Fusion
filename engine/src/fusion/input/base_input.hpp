#pragma once

#include "fusion/core/engine.hpp"

#include "mouse_buttons.hpp"
#include "key_codes.hpp"
#include "action_codes.hpp"
#include "joystick_states.hpp"

namespace fe {
    struct KeyState {
        bool pressed{ false };
        uint64_t lastFrame{ UINT64_MAX };
    };

    class Engine;

    class BaseInput {
    public:
        //! Takes a list of which keys to keep state for
        BaseInput() = default;
        virtual ~BaseInput() = default;
        NONCOPYABLE(BaseInput);

        virtual void onUpdate() = 0;

    protected:
        template<class T>
        static bool IsKey(const std::map<T, KeyState>& keys, T keycode) {
            if (auto it = keys.find(keycode); it != keys.end()) {
                const auto& key = it->second;
                return key.pressed;
            }
            return false;
        }

        template<class T>
        static bool IsKeyUp(const std::map<T, KeyState>& keys, T keycode) {
            if (auto it = keys.find(keycode); it != keys.end()) {
                const auto& key = it->second;
                return !key.pressed && key.lastFrame == Engine::Get()->frameCount();
            }
            return false;
        }

        template<class T>
        static bool IsKeyDown(const std::map<T, KeyState>& keys, T keycode) {
            if (auto it = keys.find(keycode); it != keys.end()) {
                const auto& key = it->second;
                return key.pressed && key.lastFrame == Engine::Get()->frameCount();
            }
            return false;
        }

        template<class T>
        static void SetKey(std::map<T, KeyState>& keys, T keycode, ActionCode action) {
            if (action == Action::Repeat)
                return;
            if (auto it = keys.find(keycode); it != keys.end()) {
                auto& key = it->second;
                switch (action) {
                    case Action::Press:
                        key.pressed = true;
                        key.lastFrame = Engine::Get()->frameCount();
                        break;
                    case Action::Release:
                        key.pressed = false;
                        key.lastFrame = Engine::Get()->frameCount();
                        break;
                    default: // Repeat
                        break;
                }
            }
        }

        template<class T>
        static bool IsKey(const std::vector<KeyState>& keys, T keycode) {
            if (keycode >= keys.size())
                return false;
            const auto& key = keys[keycode];
            return key.pressed;
        }

        template<class T>
        static bool IsKeyUp(const std::vector<KeyState>& keys, T keycode) {
            if (keycode >= keys.size())
                return false;
            const auto& key = keys[keycode];
            return !key.pressed && key.lastFrame == Engine::Get()->frameCount();
        }

        template<class T>
        static bool IsKeyDown(const std::vector<KeyState>& keys, T keycode) {
            if (keycode >= keys.size())
                return false;
            const auto& key = keys[keycode];
            return key.pressed && key.lastFrame == Engine::Get()->frameCount();
        }

        template<class T>
        static void SetKey(std::vector<KeyState>& keys, T keycode, ActionCode action) {
            if (action == Action::Repeat || keycode >= keys.size())
                return;
            auto& key = keys[keycode];
            switch (action) {
                case Action::Press:
                    key.pressed = true;
                    key.lastFrame = Engine::Get()->frameCount();
                    break;
                case Action::Release:
                    key.pressed = false;
                    key.lastFrame = Engine::Get()->frameCount();
                    break;
                default: // Repeat
                    break;
            }
        }
    };
}