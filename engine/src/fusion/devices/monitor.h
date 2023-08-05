#pragma once

namespace fe {
    /**
     * @brief Class used to describe the video mode for a monitor.
     */
    struct VideoMode {
        /// The width, in screen coordinates, of the video mode.
        int32_t width;
        /// The height, in screen coordinates, of the video mode.
        int32_t height;
        /// The bit depth of the red channel of the video mode.
        int32_t redBits;
        /// The bit depth of the green channel of the video mode.
        int32_t greenBits;
        /// The bit depth of the blue channel of the video mode.
        int32_t blueBits;
        /// The refresh rate, in Hz, of the video mode.
        int32_t refreshRate;
    };

    /**
     * @brief Class used to describe the gamma ramp for a monitor.
     */
    struct GammaRamp {
        /// An array of value describing the response of the red channel.
        uint16_t* red;
        /// An array of value describing the response of the green channel.
        uint16_t* green;
        /// An array of value describing the response of the blue channel.
        uint16_t* blue;
        /// The number of elements in each array.
        uint32_t size;
    };

    /**
     * @brief Class used to represent a monitor.
     */
    class FUSION_API Monitor {
        friend class DeviceManager;
    public:
        Monitor() = default;
        virtual ~Monitor() = default;
        NONCOPYABLE(Monitor);

        /**
         * Validate that monitor is primary.
         * @return True if primary, false otherwise.
         */
        virtual bool isPrimary() const = 0;

        /**
         * Gets the workarea size of the monitor.
         * @return The size, in pixels, of the display area.
         */
        virtual glm::uvec2 getWorkareaSize() const = 0;

        /**
         * Gets the workarea size of the monitor.
         * @return The size, in pixels, of the display area.
         */
        virtual glm::uvec2 getWorkareaPosition() const = 0;

        /**
         * Gets the physical size of the monitor.
         * @return The size, in millimetres, of the display area.
         */
        virtual glm::uvec2 getSize() const = 0;

        /**
         * Gets the content scale of the monitor.
         * @return The content scale, the ratio between the current DPI and the platform's default DPI.
         */
        virtual glm::vec2 getContentScale() const = 0;

        /**
         * Gets the position of the monitor's viewport on the virtual screen.
         * @return The position of the monitor's viewport.
         */
        virtual glm::uvec2 getPosition() const = 0;

        /**
         * Gets the name of this monitor.
         * @return The name of the monitor.
         */
        virtual const std::string& getName() const = 0;

        /**
         * Gets the available video modes for this monitor.
         * @return The video modes supported by this monitor.
         */
        virtual const std::vector<VideoMode>& getVideoModes() const = 0;

        /**
         * Gets the current mode of this monitor.
         * @return The current video mode of this monitor.
         */
        virtual const VideoMode& getVideoMode() const = 0;

        /**
         * Gets the current gamma ramp for this monitor.
         * @return The current gamma ramp.
         */
        virtual const GammaRamp& getGammaRamp() const = 0;

        /**
         * Sets the current gamma ramp for this monitor.
         * @param gammaRamp The gamma ramp to use.
         */
        virtual void setGammaRamp(const GammaRamp& gammaRamp) = 0;

        /**
         * Gets the current monitor object.
         * @return The object.
         */
        virtual void* getNativeMonitor() const = 0;

    protected:
        /**
         * Run every frame as long as the monitor has work to do.
         */
        virtual void onUpdate() = 0;
    };
}