#include "android_device_manager.h"
#include "android_window.h"
#include "android_engine.h"
#include "android.h"

using namespace android;

DeviceManager::DeviceManager() : fe::DeviceManager{} {
}

DeviceManager::~DeviceManager() {
}

void DeviceManager::onStart() {
    auto app = static_cast<struct android_app*>(Engine::Get()->getNativeApp());
    assetManager = app->activity->assetManager;
    nativeWindow = app->window;
}

void DeviceManager::onUpdate() {
}

void DeviceManager::waitEvents() {
}

std::vector<const char*> DeviceManager::getRequiredInstanceExtensions() const {
    return { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME };
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
    if (!windows.empty())
        return nullptr;

    auto& it = windows.emplace_back(std::make_unique<android::Window>(nativeWindow, windowInfo));
    onWindowCreate.publish(it.get(), true);
    return it.get();
}

fe::Cursor* DeviceManager::createCursor(const fs::path& filepath, fe::CursorHotspot hotspot) {
    return nullptr;
}