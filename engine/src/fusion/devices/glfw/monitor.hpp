#pragma once

#include "fusion/devices/monitor.hpp"
#include "glm/fwd.hpp"

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

namespace glfw {
    class Monitor : public fe::Monitor {
    public:
        Monitor(GLFWmonitor* monitor);
        ~Monitor();

        bool isPrimary() const override{ return monitor == glfwGetPrimaryMonitor(); }

        glm::uvec4 getWorkarea() const;
        glm::uvec2 getWorkareaSize() const override;
        glm::uvec2 getWorkareaPosition() const override;
        glm::uvec2 getSize() const override;
        glm::vec2 getContentScale() const override;
        glm::uvec2 getPosition() const override;
        const std::string& getName() const override { return name; }

        const std::vector<fe::VideoMode>& getVideoModes() const override { return modes; }
        const fe::VideoMode& getVideoMode() const override;

        const fe::GammaRamp& getGammaRamp() const override;
        void setGammaRamp(const fe::GammaRamp& gammaRamp) override;

        void* getNativeMonitor() const override { return monitor; }

    private:
        GLFWmonitor* monitor;
        std::string name;
        std::vector<fe::VideoMode> modes;
    };
}