#pragma once

#include "fusion/core/module.hpp"
#include "fusion/utils/date_time.hpp"

namespace fe {
    class Time : public Module::Registrar<Time> {
    public:
        Time() { lastTime = frameTime = DateTime::Now(); }
        ~Time() override = default;

        //! The time at the beginning of this frame (Read Only).
        static DateTime CurrentTime() { return ModuleInstance->lastTime; }
        //! The interval in seconds from the last frame to the current one.
        static DateTime DeltaTime() { return ModuleInstance->deltaTime; }
        //! The total number of frames since the start of the game.
        static uint64_t FrameCount() { return ModuleInstance->frameCount; }
        //! Incremented once per frame before the scene is being rendered. Reset on the each second
        static uint32_t FrameNumber() { return ModuleInstance->frameNumber; }
        //! The number of frames per second
        static uint32_t FramesPerSecond() { return ModuleInstance->framesPerSecond; }

        void onUpdate() override {
            auto currentTime = DateTime::Now();
            deltaTime = (currentTime - lastTime);
            lastTime = currentTime;

            frameCount++;

            frameNumber++;
            if ((currentTime - frameTime).asSeconds() >= 1) {
                framesPerSecond = frameNumber;
                frameNumber = 0;
                frameTime = currentTime;
            }
        }

    private:
        DateTime deltaTime;
        DateTime lastTime;
        DateTime frameTime;
        uint64_t frameCount{ 0 };
        uint32_t frameNumber{ 0 };
        uint32_t framesPerSecond{ 0 };
    };
}
