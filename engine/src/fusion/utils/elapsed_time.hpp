#pragma once

#include "time.hpp"

namespace fe {
    class ElapsedTime {
    public:
        explicit ElapsedTime(const Time& interval = -1s);

        uint32_t getElapsed();

        const Time& getStartTime() const { return startTime; }
        void setStartTime(const Time& time) { startTime = time; }

        const Time& getInterval() const { return interval; }
        void setInterval(const Time& time) { interval = time; }

    private:
        Time startTime;
        Time interval;
    };
}
