#pragma once

#include "fusion/devices/device_manager.h"

class ANativeWindow;
class AAssetManager;

namespace android {
    class FUSION_API DeviceManager : public fe::DeviceManager {
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

        void* getNativeManager() const override { return assetManager; }

    protected:
        void onStart() override;
        void onUpdate() override;
        void onStop() override {};

    private:
        AAssetManager* assetManager{ nullptr };
        ANativeWindow* nativeWindow{ nullptr };
    };
}
