#pragma once

#include "Core.hpp"

namespace Fusion {
    class Window;

    class FUSION_API Input {
    public:
        Input() = default;
        virtual ~Input() = default;

        virtual void onUpdate() = 0;

        virtual bool getKey(int keycode) = 0;
        virtual bool getKeyDown(int keycode) = 0;

        virtual bool getMouseButton(int button) = 0;
        virtual bool getMouseButtonDown(int button) = 0;

        virtual const glm::vec2& mousePosition() = 0;
        virtual const glm::vec2& mouseDelta() = 0;

        static Input* create(Window& window);
    };
}