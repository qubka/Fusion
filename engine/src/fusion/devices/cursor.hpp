#pragma once

namespace fe {
    enum class CursorHotspot : uint8_t {
        UpperLeft = 0,
        UpperRight,
        BottomLeft,
        BottomRight,
        Centered
    };

    enum class CursorStandard : uint8_t {
        None = 0,
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
         * Gets the cursor path to image.
         * @return The cursor image.
         */
        virtual const fs::path& getPath() const = 0;

        /**
         * Gets the cursor hotspot.
         * @return The cursor hotspot.
         */
        virtual CursorHotspot getHotspot() const = 0;

        /**
         * Gets the cursor standart.
         * @return The cursor standart.
         */
        virtual CursorStandard getStandard() const = 0;

        /**
         * Gets the current cursor object.
         * @return The object.
         */
        virtual void* getNativeCursor() const = 0;
    };
}





