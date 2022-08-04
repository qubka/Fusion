#include "elapsed_time.hpp"

using namespace fe;

ElapsedTime::ElapsedTime(const DateTime& interval) : startTime{DateTime::Now()}, interval{interval} {
}

uint32_t ElapsedTime::getElapsed() {
    auto now = DateTime::Now();
    auto elapsed = static_cast<uint32_t>(glm::floor((now - startTime) / interval));

    if (elapsed != 0) {
        startTime = now;
    }

    return elapsed;
}
