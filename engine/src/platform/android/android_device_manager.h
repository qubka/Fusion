#pragma once

#include "fusion/devices/device_manager.h"

namespace android {
    class DeviceManager : public fe::DeviceManager {
        friend class Engine;
    public:
        DeviceManager();
        ~DeviceManager() override;

        void waitEvents() override;

        fe::Key getScanCode(fe::Key key) const override;
        std::string getKeyName(fe::Key key, fe::Key scancode) const override;

        std::vector<const char*> getRequiredInstanceExtensions() const override;
        bool isRawMouseMotionSupported() const override;
        void updateGamepadMappings(std::string_view mappings) override;

        fe::Window* createWindow(const fe::WindowInfo& windowInfo) override;
        fe::Cursor* createCursor(const fs::path& filepath, fe::CursorHotspot hotspot) override;

    protected:
        void onUpdate() override;
    };
}
