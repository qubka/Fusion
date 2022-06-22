#include "device_manager.hpp"
#include "monitor.hpp"
#include "joystick.hpp"
#include "window.hpp"

using namespace fe;

DeviceManager* DeviceManager::Instance{ nullptr };

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