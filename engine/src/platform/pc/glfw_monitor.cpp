#include "glfw_monitor.h"
#include "glfw_device_manager.h"

using namespace glfw;

Monitor::Monitor(GLFWmonitor* monitor) : monitor{monitor}, name{glfwGetMonitorName(monitor)} {
    glfwSetMonitorUserPointer(monitor, this);

    int videoModeCount;
    auto videoModes = glfwGetVideoModes(monitor, &videoModeCount);

    //std::vector<fe::VideoMode> modes;
    modes.reserve(videoModeCount);

    for (int i = 0; i < videoModeCount; i++)
        modes.push_back(*reinterpret_cast<const fe::VideoMode*>(&videoModes[i]));
}

glm::uvec4 Monitor::getWorkarea() const {
    glm::ivec4 workarea;
    glfwGetMonitorWorkarea(monitor, &workarea.x, &workarea.y, &workarea.z, &workarea.w);
    return workarea;
}

glm::uvec2 Monitor::getWorkareaSize() const {
    glm::ivec2 size;
    glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &size.x, &size.y);
    return size;
}

glm::uvec2 Monitor::getWorkareaPosition() const {
    glm::ivec2 pos;
    glfwGetMonitorWorkarea(monitor, &pos.x, &pos.y, nullptr, nullptr);
    return pos;
}

glm::uvec2 Monitor::getSize() const {
    glm::ivec2 size;
    glfwGetMonitorPhysicalSize(monitor, &size.x, &size.y);
    return size;
}

glm::vec2 Monitor::getContentScale() const {
    glm::vec2 scale;
    glfwGetMonitorContentScale(monitor, &scale.x, &scale.y);
    return scale;
}

glm::uvec2 Monitor::getPosition() const {
    glm::ivec2 pos;
    glfwGetMonitorPos(monitor, &pos.x, &pos.y);
    return pos;
}

const fe::VideoMode& Monitor::getVideoMode() const {
    auto videoMode = glfwGetVideoMode(monitor);
    return *reinterpret_cast<const fe::VideoMode*>(videoMode);
}

const fe::GammaRamp& Monitor::getGammaRamp() const {
    auto gamaRamp = glfwGetGammaRamp(monitor);
    return *reinterpret_cast<const fe::GammaRamp*>(gamaRamp);
}

void Monitor::setGammaRamp(const fe::GammaRamp& gammaRamp) {
    auto ramp = reinterpret_cast<const GLFWgammaramp*>(&gammaRamp);
    glfwSetGammaRamp(monitor, ramp);
}
