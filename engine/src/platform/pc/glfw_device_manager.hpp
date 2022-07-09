#pragma once

#include "fusion/devices/device_manager.hpp"

namespace glfw {
    class DeviceManager : public fe::DeviceManager {
    public:
        DeviceManager();
        ~DeviceManager() override;

        void onUpdate() override;
        void waitEvents() override;

        fe::Key getScanCode(fe::Key key) const override;
        std::string getKeyName(fe::Key key, fe::Key scancode) const override;

        std::vector<const char*> getRequiredInstanceExtensions() const override;
        bool isRawMouseMotionSupported() const override;
        void updateGamepadMappings(const std::string& mappings) override;

        fe::Window* createWindow(const fe::WindowInfo& windowInfo) override;

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
