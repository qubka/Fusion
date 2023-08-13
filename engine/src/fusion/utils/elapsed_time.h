#pragma once

#include "fusion/utils/date_time.h"

namespace fe {
    class FUSION_API ElapsedTime {
    public:
        explicit ElapsedTime(DateTime interval = -1s);

        uint32_t getElapsed() const;

        const DateTime& getInterval() const { return interval; }
        void setInterval(const DateTime& time) { interval = time; }

        const DateTime& getStartTime() const { return startTime; }
        void setStartTime(const DateTime& time) { startTime = time; }

    private:
        DateTime interval;
        mutable DateTime startTime;
    };
}
