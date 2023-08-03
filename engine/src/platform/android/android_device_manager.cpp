#include "android_device_manager.h"

using namespace android;

DeviceManager::DeviceManager() : fe::DeviceManager{} {
}

DeviceManager::~DeviceManager() {
}

void DeviceManager::onUpdate() {
}

void DeviceManager::waitEvents() {
}

std::vector<const char*> DeviceManager::getRequiredInstanceExtensions() const {
    return {};
}

fe::Key DeviceManager::getScanCode(fe::Key key) const{
    return key;
}

std::string DeviceManager::getKeyName(fe::Key key, fe::Key scancode) const {
    return "";
}

bool DeviceManager::isRawMouseMotionSupported() const {
    return false;
}

void DeviceManager::updateGamepadMappings(std::string_view mappings) {
}

fe::Window* DeviceManager::createWindow(const fe::WindowInfo& windowInfo) {
    return nullptr;
}

fe::Cursor* DeviceManager::createCursor(const fs::path& filepath, fe::CursorHotspot hotspot) {
    return nullptr;
}