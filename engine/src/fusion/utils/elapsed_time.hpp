#pragma once

#include "fusion/utils/date_time.hpp"

namespace fe {
    class ElapsedTime {
    public:
        explicit ElapsedTime(const DateTime& interval = -1s);

        uint32_t getElapsed();

        const DateTime& getStartTime() const { return startTime; }
        void setStartTime(const DateTime& time) { startTime = time; }

        const DateTime& getInterval() const { return interval; }
        void setInterval(const DateTime& time) { interval = time; }

    private:
        DateTime startTime;
        DateTime interval;
    };
}
