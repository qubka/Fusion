#include "time.h"

using namespace fe;

Time* Time::Instance = nullptr;

Time::Time() {
    Instance = this;
}

Time::~Time() {
    Instance = nullptr;
}

void Time::onStart() {
    lastTime = frameTime = DateTime::Now();
}

void Time::onUpdate() {
    auto currentTime = DateTime::Now();
    deltaTime = (currentTime - lastTime);
    lastTime = currentTime;

    ++frameCount;
    ++frameNumber;

    if ((currentTime - frameTime).asSeconds() >= 1) {
        framesPerSecond = frameNumber;
        frameNumber = 0;
        frameTime = currentTime;
    }
}

void Time::onStop() {
}

