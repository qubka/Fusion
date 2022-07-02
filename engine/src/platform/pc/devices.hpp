#pragma once

#include "fusion/devices/devices.hpp"

namespace glfw {
    class Devices : public fe::Devices {
    public:
        Devices();
        ~Devices() override;

        void update() override;
        void waitEvents() override;

        fe::Key getScanCode(fe::Key key) const override;
        std::string getKeyName(fe::Key key, fe::Key scancode) const override;

        std::vector<const char*> getRequiredInstanceExtensions() const override;
        bool isRawMouseMotionSupported() const override;
        void updateGamepadMappings(const std::string& mappings) override;

    private:
#if GLFW_VERSION_MINOR >= 2
        static void JoystickCallback(int jid, int action);
#endif
        static void ErrorCallback(int error, const char* description);
        static void MonitorCallback(GLFWmonitor* monitor, int action);

        static std::string StringifyResultGlfw(int result);
        static void CheckGlfw(int result);
    };
}
