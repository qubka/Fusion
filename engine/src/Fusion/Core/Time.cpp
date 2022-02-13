#include "Time.hpp"

#include <GLFW/glfw3.h>

using namespace Fusion;

double Time::time{};
float Time::elapsedTime{};
double Time::frameTime{};
uint64_t Time::frameCount{};
uint32_t Time::frameNumber{};
uint32_t Time::framesPerSecond{};

void Time::Update() {
    double currentTime = glfwGetTime();
    elapsedTime = static_cast<float>(currentTime - time);
    time = currentTime;

    frameCount++;

    frameNumber++;
    if (currentTime - frameTime >= 1) {
        framesPerSecond = frameNumber;
        frameNumber = 0;
        frameTime = currentTime;
    }
}
