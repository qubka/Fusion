#include "device_manager.hpp"
#include "monitor.hpp"
#include "window.hpp"
#include "joystick.hpp"

using namespace glfw;

DeviceManager::DeviceManager() : fe::Devices{} {
    // Set the error error callback.
    glfwSetErrorCallback(ErrorCallback);

    // Initialize the GLFW library.
    if (glfwInit() == GLFW_FALSE)
        throw std::runtime_error("GLFW failed to initialize");

    // Checks Vulkan support on GLFW.
    if (glfwVulkanSupported() == GLFW_FALSE)
        throw std::runtime_error("GLFW failed to find Vulkan support");

    // Set the monitor callback
    glfwSetMonitorCallback(MonitorCallback);

#if GLFW_VERSION_MINOR >= 2
    // Set the joystick callback
    glfwSetJoystickCallback(JoystickCallback);
#endif

    // The window will stay hidden until after creation.
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    // Disable context creation.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Fixes 16 bit stencil bits in macOS.
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    // No stereo view!
    glfwWindowHint(GLFW_STEREO, GLFW_FALSE);

    // Get connected monitors.
    int count;
    auto glfwMonitors = glfwGetMonitors(&count);
    for (int i = 0; i < count; i++)
        monitors.push_back(std::make_unique<Monitor>(glfwMonitors[i]));

    createWindow<glfw::Window>(glm::uvec2{1280, 720});
}

DeviceManager::~DeviceManager() {
    // Terminate GLFW.
    glfwTerminate();
}

void DeviceManager::update() {
    // Polls for window events.
    glfwPollEvents();

    for (auto& w : windows) {
        w->update();
    }

    for (auto& j : joysticks) {
        j->update();
    }
}

void DeviceManager::waitEvents() {
    // Wait for GLFW events.
    glfwWaitEvents();
}

std::vector<const char*> DeviceManager::getRequiredInstanceExtensions() const {
    std::vector<const char*> result;
    uint32_t count = 0;
    const char** names = glfwGetRequiredInstanceExtensions(&count);
    if (names && count) {
        for (uint32_t i = 0; i < count; ++i) {
            result.push_back(names[i]);
        }
    }
    return result;
}

fe::ScanCode DeviceManager::getScanCode(fe::KeyCode key) const{
    return glfwGetKeyScancode(key);
}

std::string DeviceManager::getKeyName(fe::KeyCode key, fe::ScanCode scancode) const {
    return glfwGetKeyName(key, scancode);
}

bool DeviceManager::isRawMouseMotionSupported() const {
    return glfwRawMouseMotionSupported() == GLFW_TRUE;
}

void DeviceManager::updateGamepadMappings(const std::string& mappings) {
    glfwUpdateGamepadMappings(mappings.c_str());
}

/* Events Callbacks */

namespace glfw {

    void DeviceManager::ErrorCallback(int error, const char* description) {
        CheckGlfw(error);
    }

    void DeviceManager::MonitorCallback(GLFWmonitor* monitor, int action) {
        auto manager = reinterpret_cast<glfw::DeviceManager*>(DeviceManager::Get());
        auto& monitors = manager->monitors;

        LOG_VERBOSE << "MonitorEvent: name: [" << glfwGetMonitorName(monitor) << "] - " << (action == GLFW_CONNECTED ? "Connected" : "Disconnected");

        if (action == GLFW_CONNECTED) {
            auto& it = monitors.emplace_back(std::make_unique<Monitor>(monitor));
            manager->onMonitorConnect.publish(it.get(), true);
        } else if (action == GLFW_DISCONNECTED) {
            auto it = std::find_if(monitors.begin(), monitors.end(), [monitor](const auto& m) {
                return monitor == m->getNativeMonitor();
            });
            if (it != monitors.end()) {
                manager->onMonitorConnect.publish(it->get(), false);
                monitors.erase(it);
            }
        }
    }

    #if GLFW_VERSION_MINOR >= 2
    void DeviceManager::JoystickCallback(int jid, int action) {
        auto manager = reinterpret_cast<glfw::DeviceManager*>(DeviceManager::Get());
        auto& joysticks = manager->joysticks;

        LOG_VERBOSE << "JoystickEvent: id: [" << jid << "] | name: [" << glfwGetJoystickName(jid) << "] - " << (action == GLFW_CONNECTED ? "Connected" : "Disconnected");

        if (action == GLFW_CONNECTED) {
            auto& it = joysticks.emplace_back(std::make_unique<Joystick>(jid));
            manager->onJoystickConnect.publish(it.get(), true);
        } else if (action == GLFW_DISCONNECTED) {
            auto it = std::find_if(joysticks.begin(), joysticks.end(), [jid](const auto& j) {
                return jid == j->getPort();
            });
            if (it != joysticks.end()) {
                manager->onJoystickConnect.publish(it->get(), false);
                joysticks.erase(it);
            }
        }
    }
    #endif

    std::string DeviceManager::StringifyResultGlfw(int result) {
        switch (result) {
            case GLFW_TRUE:
                return "Success";
            case GLFW_NOT_INITIALIZED:
                return "GLFW has not been initialized";
            case GLFW_NO_CURRENT_CONTEXT:
                return "No context is current for this thread";
            case GLFW_INVALID_ENUM:
                return "One of the arguments to the function was an invalid enum value";
            case GLFW_INVALID_VALUE:
                return "One of the arguments to the function was an invalid value";
            case GLFW_OUT_OF_MEMORY:
                return "A memory allocation failed";
            case GLFW_API_UNAVAILABLE:
                return "GLFW could not find support for the requested API on the system";
            case GLFW_VERSION_UNAVAILABLE:
                return "The requested OpenGL or OpenGL ES version is not available";
            case GLFW_PLATFORM_ERROR:
                return "A platform-specific error occurred that does not match any of the more specific categories";
            case GLFW_FORMAT_UNAVAILABLE:
                return "The requested format is not supported or available";
            case GLFW_NO_WINDOW_CONTEXT:
                return "The specified window does not have an OpenGL or OpenGL ES context";
            default:
                return "ERROR: UNKNOWN GLFW ERROR";
        }
    }

    void DeviceManager::CheckGlfw(int result) {
        if (result) return;
        auto failure = StringifyResultGlfw(result);
        LOG_ERROR << "GLFW error: " << failure << ", " << result;
        throw std::runtime_error("GLFW error: " + failure);
    }
}