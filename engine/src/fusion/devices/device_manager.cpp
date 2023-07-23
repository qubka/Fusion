#include "device_manager.h"
#include "monitor.h"
#include "joystick.h"

#if FUSION_PLATFORM_ANDROID
#include "platform/android/android_device_manager.h"
#else
#include "platform/pc/glfw_device_manager.h"
#endif

using namespace fe;

DeviceManager* DeviceManager::Instance = nullptr;

DeviceManager::DeviceManager() {
    Instance = this;
}

DeviceManager::~DeviceManager() {
    Instance = nullptr;
}

const Monitor* DeviceManager::getPrimaryMonitor() {
    for (const auto& monitor : monitors) {
        if (monitor->isPrimary())
            return monitor.get();
    }
    return nullptr;
}

std::unique_ptr<DeviceManager> DeviceManager::Init() {
    if (Instance != nullptr)
        throw std::runtime_error("Device Manager already instantiated!");

#if FUSION_PLATFORM_ANDROID
    return std::make_unique<android::Devices>();
#elif FUSION_PLATFORM_LINUX || FUSION_PLATFORM_WINDOWS || FUSION_PLATFORM_MAC
    return std::make_unique<glfw::DeviceManager>();
#else
    LOG_FATAL << "Unknown platform!";
    return nullptr;
#endif
}