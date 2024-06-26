#pragma once

#include "fusion/devices/device_manager.h"

extern "C" {
    typedef struct GLFWmonitor GLFWmonitor;
}

namespace fe::glfw {
    class FUSION_API DeviceManager final : public fe::DeviceManager {
        friend class Engine;
    public:
        DeviceManager();
        ~DeviceManager() override;

        void waitEvents() override;

        Key getScanCode(Key key) const override;
        std::string getKeyName(Key key, Key scancode) const override;

        std::vector<const char*> getRequiredInstanceExtensions() const override;
        bool isRawMouseMotionSupported() const override;
        void updateGamepadMappings(std::string_view mappings) override;

        Window* createWindow(const WindowInfo& windowInfo) override;
        Cursor* createCursor(const fs::path& filepath, CursorHotspot hotspot) override;

        void* getNativeManager() const override { return (void*) this; }

    protected:
        void onStart() override {}
        void onUpdate() override;
        void onStop() override {}

    private:
        static void JoystickCallback(int jid, int action);

        static void ErrorCallback(int error, const char* description);
        static void MonitorCallback(GLFWmonitor* monitor, int action);

        static std::string StringifyResultGlfw(int result);
        static void CheckGlfw(int result);
    };
}
