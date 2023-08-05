#pragma once

#include "fusion/devices/cursor.h"

extern "C" {
    typedef struct GLFWcursor GLFWcursor;
}

namespace glfw {
    class FUSION_API Cursor : public fe::Cursor {
    public:
        /**
         * Creates the cursor from a image file.
         * @param filepath The new custom mouse file.
         * @param hotspot The hotspot to display the cursor image at.
         */
        Cursor(fs::path filepath, fe::CursorHotspot hotspot);

        /**
         * Creates the cursor with a system style.
         * @param standard The standard shape.
         */
        explicit Cursor(fe::CursorStandard standard);

        Cursor() = default;
        ~Cursor() override;

        const fs::path& getPath() const override { return path; };
        fe::CursorHotspot getHotspot() const override { return hotspot; };
        fe::CursorStandard getStandard() const override { return standard; };
        void* getNativeCursor() const override { return cursor; }

    private:
        GLFWcursor* cursor{ nullptr };

        fs::path path;
        fe::CursorHotspot hotspot{ fe::CursorHotspot::Centered };
        fe::CursorStandard standard{ fe::CursorStandard::None };
    };
}
