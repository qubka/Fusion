#include "elapsed_time.h"

using namespace fe;

ElapsedTime::ElapsedTime(DateTime interval) : startTime{DateTime::Now()}, interval{interval} {
}

uint32_t ElapsedTime::getElapsed() const {
    auto now = DateTime::Now();
    auto elapsed = static_cast<uint32_t>(glm::floor((now - startTime) / interval));

    if (elapsed != 0) {
        startTime = now;
    }

    return elapsed;
}
