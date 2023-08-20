#pragma once

#include "fusion/devices/cursor.h"

extern "C" {
    typedef struct GLFWcursor GLFWcursor;
}

namespace fe::glfw {
    class FUSION_API Cursor final : public fe::Cursor {
    public:
        /**
         * Creates the cursor from a image file.
         * @param filepath The new custom mouse file.
         * @param hotspot The hotspot to display the cursor image at.
         */
        Cursor(fs::path filepath, CursorHotspot hotspot);

        /**
         * Creates the cursor with a system style.
         * @param standard The standard shape.
         */
        explicit Cursor(CursorStandard standard);

        Cursor() = default;
        ~Cursor() override;

        const fs::path& getPath() const override { return path; };
        CursorHotspot getHotspot() const override { return hotspot; };
        CursorStandard getStandard() const override { return standard; };
        void* getNativeCursor() const override { return cursor; }

    private:
        GLFWcursor* cursor{ nullptr };

        fs::path path;
        CursorHotspot hotspot{ CursorHotspot::Centered };
        CursorStandard standard{ CursorStandard::None };
    };
}
