#pragma once

#include "fusion/utils/date_time.h"

namespace fe {
    template<typename T>
    class Module;

    class FUSION_API Time {
        friend class Module<Time>;
    private:
        Time();
        ~Time();

    public:
        static Time* Get() { return Instance; }

        //! The time at the beginning of this frame (Read Only).
        static DateTime CurrentTime() { return Instance->lastTime; }
        //! The interval in seconds from the last frame to the current one.
        static DateTime DeltaTime() { return Instance->deltaTime; }
        //! The total number of frames since the start of the game.
        static uint64_t FrameCount() { return Instance->frameCount; }
        //! Incremented once per frame before the scene is being rendered. Reset on the each second
        static uint32_t FrameNumber() { return Instance->frameNumber; }
        //! The number of frames per second
        static uint32_t FramesPerSecond() { return Instance->framesPerSecond; }

    private:
        void onStart();
        void onUpdate();
        void onStop();

    private:
        DateTime deltaTime;
        DateTime lastTime;
        DateTime frameTime;
        uint64_t frameCount{ 0 };
        uint32_t frameNumber{ 0 };
        uint32_t framesPerSecond{ 0 };

        static Time* Instance;
    };
}
