#include "android_device_manager.h"
#include "android_window.h"
#include "android.h"

using namespace fe::android;

DeviceManager::DeviceManager() : fe::DeviceManager{} {
}

DeviceManager::~DeviceManager() {
}

void DeviceManager::onUpdate() {
}

void DeviceManager::waitEvents() {
}

std::vector<const char*> DeviceManager::getRequiredInstanceExtensions() const {
    return { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME };
}

fe::Key DeviceManager::getScanCode(Key key) const{
    return key;
}

std::string DeviceManager::getKeyName(Key key, Key scancode) const {
    return "";
}

bool DeviceManager::isRawMouseMotionSupported() const {
    return false;
}

void DeviceManager::updateGamepadMappings(std::string_view mappings) {
}

fe::Window* DeviceManager::createWindow(const WindowInfo& windowInfo) {
    if (!windows.empty())
        return nullptr;

    auto& it = windows.emplace_back(std::make_unique<android::Window>(windowInfo));
    onWindowCreate.publish(it.get(), true);
    return it.get();
}

fe::Cursor* DeviceManager::createCursor(const fs::path& filepath, CursorHotspot hotspot) {
    return nullptr;
}