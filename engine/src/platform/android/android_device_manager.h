#pragma once

#include "fusion/devices/device_manager.h"

namespace  fe::android {
    class FUSION_API DeviceManager : public fe::DeviceManager {
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

        void* getNativeManager() const override { return (void*)this; }

    protected:
        void onStart() override {};
        void onUpdate() override;
        void onStop() override {};

    private:
    };
}
