#include "devices.hpp"
#include "monitor.hpp"
#include "joystick.hpp"

#if PLATFORM_ANDROID
#include "platform/android/device_manager.hpp"
#else
#include "platform/pc/devices.hpp"
#endif

using namespace fe;

Devices* Devices::Instance{ nullptr };

Devices::Devices() {
    Instance = this;
}

Devices::~Devices() {
    Instance = nullptr;
}

const Monitor* Devices::getPrimaryMonitor() {
    for (const auto& monitor : monitors) {
        if (monitor->isPrimary())
            return monitor.get();
    }
    return nullptr;
}

std::unique_ptr<Devices> Devices::Init() {
    if (Instance != nullptr)
        throw std::runtime_error("Device Manager already instantiated!");

#if PLATFORM_ANDROID
    return std::make_unique<android::Devices>();
#elif PLATFORM_LINUX || PLATFORM_WINDOWS || PLATFORM_MAC
    return std::make_unique<glfw::Devices>();
#else
    LOG_FATAL << "Unknown platform!";
    return nullptr;
#endif
}