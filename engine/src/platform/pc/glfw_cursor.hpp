#pragma once

#include "fusion/devices/cursor.hpp"

#include <GLFW/glfw3.h>

namespace glfw {
    class Cursor : fe::Cursor {
    public:
        /**
         * Creates the cursor from a image file.
         * @param filename The new custom mouse file.
         * @param hotspot The hotspot to display the cursor image at.
         */
        Cursor(const fs::path& filename, fe::CursorHotspot hotspot);

        /**
         * Creates the cursor with a system style.
         * @param standard The standard shape.
         */
        explicit Cursor(fe::CursorStandard standard);

        ~Cursor() override;

        void* getNativeCursor() const override { return cursor; }

    private:
        GLFWcursor* cursor{ nullptr };
    };
}
