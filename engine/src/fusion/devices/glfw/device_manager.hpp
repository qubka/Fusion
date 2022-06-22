#pragma once

#include "fusion/devices/device_manager.hpp"

namespace glfw {
    class DeviceManager : public fe::DeviceManager {
    public:
        DeviceManager();
        ~DeviceManager() override;

        void update() override;
        void wait() override;

        fe::ScanCode getScanCode(fe::KeyCode key) const override;
        std::string getKeyName(fe::KeyCode key, fe::ScanCode scancode) const override;

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
