#pragma once

#include "Base.hpp"

namespace Fusion {
    //! Global time holder
    class FUSION_API Time {
    public:
        //! The time at the beginning of this frame (Read Only).
        static float TimeAsFloat() { return static_cast<float>(time); }
        //! The double precision time at the beginning of this frame. This is the time in seconds since the start of the game.
        static double TimeAsDouble() { return time; }
        //! The interval in seconds from the last frame to the current one.
        static float ElapsedTime() { return elapsedTime; }
        //! The total number of frames since the start of the game.
        static uint64_t FrameCount() { return frameCount; }
        //! Incremented once per frame before the scene is being rendered. Reset on the each second
        static uint32_t FrameNumber() { return frameNumber; }
        //! The number of frames per second
        static uint32_t FramesPerSecond() { return framesPerSecond; }

    private:
        //! Should be called in the application run loop
        static void Tick();

        static double time;
        static float elapsedTime;
        static double frameTime;
        static uint64_t frameCount;
        static uint32_t frameNumber;
        static uint32_t framesPerSecond;

        friend class Application;
    };
}
