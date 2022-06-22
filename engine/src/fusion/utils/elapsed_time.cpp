#include "elapsed_time.hpp"

using namespace fe;

ElapsedTime::ElapsedTime(const Time& interval) : startTime{Time::Now()}, interval{interval} {
}

uint32_t ElapsedTime::getElapsed() {
    auto now = Time::Now();
    auto elapsed = static_cast<uint32_t>(std::floor((now - startTime) / interval));

    if (elapsed != 0) {
        startTime = now;
    }

    return elapsed;
}
