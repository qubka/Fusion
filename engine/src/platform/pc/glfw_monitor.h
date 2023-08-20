#pragma once

#include "fusion/devices/monitor.h"

extern "C" {
    typedef struct GLFWmonitor GLFWmonitor;
}

namespace fe::glfw {
    class FUSION_API Monitor final : public fe::Monitor {
        friend class DeviceManager;
    public:
        explicit Monitor(GLFWmonitor* monitor);
        ~Monitor() override = default;

        bool isPrimary() const override;

        glm::uvec4 getWorkarea() const;
        glm::uvec2 getWorkareaSize() const override;
        glm::uvec2 getWorkareaPosition() const override;
        glm::uvec2 getSize() const override;
        glm::vec2 getContentScale() const override;
        glm::uvec2 getPosition() const override;
        const std::string& getName() const override { return name; }

        const std::vector<VideoMode>& getVideoModes() const override { return modes; }
        const VideoMode& getVideoMode() const override;

        const GammaRamp& getGammaRamp() const override;
        void setGammaRamp(const GammaRamp& gammaRamp) override;

        void* getNativeMonitor() const override { return monitor; }

    protected:
        void onUpdate() override {};

    private:
        GLFWmonitor* monitor;
        std::string name;
        std::vector<VideoMode> modes;
    };
}