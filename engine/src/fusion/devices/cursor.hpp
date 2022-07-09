#pragma once

namespace fe {
    enum class CursorHotspot {
        UpperLeft = 0,
        UpperRight,
        BottomLeft,
        BottomRight,
        Centered
    };

    enum class CursorStandard {
        Arrow = 1,
        IBeam = 2,
        Crosshair = 3,
        Hand = 4,
        ResizeEW = 5,
        ResizeNS = 6,
        ResizeNWSE = 7,
        ResizeNESW = 8,
        ResizeAll = 9,
        NotAllowed = 10
    };

    /**
     * @brief Class used to represent a cursor.
     */
    class Cursor {
    public:
        Cursor() = default;
        virtual ~Cursor() = default;
        NONCOPYABLE(Cursor);

        /**
         * Gets the current cursor object.
         * @return The object.
         */
        virtual void* getNativeCursor() const = 0;
    };
}





