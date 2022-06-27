#pragma once

#include "monitor.hpp"
#include "window.hpp"
#include "joystick.hpp"

#include "fusion/input/key_codes.hpp"

#include <GLFW/glfw3.h>

#include <entt/signal/sigh.hpp>

namespace fe {
    class Time;
    /**
     * @brief Module used for managing some devices.
     */
    class Devices {
    public:
        Devices();
        virtual ~Devices();
        NONCOPYABLE(Devices);

        static std::unique_ptr<Devices> Init();

        static Devices* Get() { return Instance; }

        /**
         * Run every frame to processes connected devices.
         */
        virtual void update() = 0;

        /**
         * Waits until events are queued and processes them.
         */
        virtual void waitEvents() = 0;

        /**
         * This function returns the platform-specific scancode of the specified key.
         * @param key Any named key.
         * @return The platform-specific scancode for the key, or 255 if an error occurred.
         */
        virtual ScanCode getScanCode(KeyCode key) const = 0;

        /**
         * This function returns the name of the specified printable key, encoded as UTF-8.
         * @param key The key to query.
         * @param scancode The scancode of the key to query.
         * @return The UTF-8 encoded, layout-specific name of the key, or NULL.
         */
        virtual std::string getKeyName(KeyCode key, ScanCode scancode) const = 0;

        /**
         * Returns the Vulkan instance extensions list required by that type of window.
         */
        virtual std::vector<const char*> getRequiredInstanceExtensions() const = 0;

        /**
         * This function returns whether raw mouse motion is supported on the current system.
         */
        virtual bool isRawMouseMotionSupported() const = 0;

        /**
        * This function parses the specified ASCII encoded string and updates the internal list with any gamepad mappings it finds.
        * This string may contain either a single gamepad mapping or many mappings separated by newlines.
        * The parser supports the full format of the gamecontrollerdb.txt source file including empty lines and comments.
        * @param mappings The string containing the gamepad mappings.
        */
        virtual void updateGamepadMappings(const std::string& mappings) = 0;

        template<typename T = Window, typename... Args>
        void createWindow(Args&&...args) {
            auto& it = windows.emplace_back(std::make_unique<T>(getPrimaryMonitor()->getVideoMode(), std::forward<Args>(args)...));
            onWindowCreate.publish(it.get(), true);
        }
        void destroyWindow(const Window* window) {
            auto it = std::find_if(windows.begin(), windows.end(), [window](const auto& w) {
                return window == w->getNativeWindow();
            });
            if (it != windows.end()) {
                onWindowCreate.publish(it->get(), false);
                windows.erase(it);
            }
        }

        const Window* getWindow(size_t id) const { return id < windows.size() ? windows[id].get() : nullptr; }
        Window* getWindow(size_t id) { return id < windows.size() ? windows[id].get() : nullptr; }

        const Monitor* getPrimaryMonitor();
        const Monitor* getMonitor(size_t id) const { return id < monitors.size() ? monitors[id].get() : nullptr; }
        Monitor* getMonitor(size_t id) { return id < monitors.size() ? monitors[id].get() : nullptr; }

        const Joystick* getJoystick(size_t id) const { return id < joysticks.size() ? joysticks[id].get() : nullptr; }
        Joystick* getJoystick(size_t id) { return id < joysticks.size() ? joysticks[id].get() : nullptr; }

        const std::vector<std::unique_ptr<Window>>& getWindows() const { return windows; };
        const std::vector<std::unique_ptr<Monitor>>& getMonitors() const { return monitors; };
        const std::vector<std::unique_ptr<Joystick>>& getJoystick() const { return joysticks; };

    public:
        /**
         * Event when a window has been created or destroy.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnWindowCreate() { return entt::sink{onWindowCreate}; }

        /**
         * Event when a monitor has been connected or disconnected.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnMonitorConnect() { return entt::sink{onMonitorConnect}; }

        /**
         * Event when a joystick has been connected or disconnected.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnJoystickConnect() { return entt::sink{onJoystickConnect}; }

    protected:
        static Devices* Instance;

        std::vector<std::unique_ptr<Window>> windows{};
        std::vector<std::unique_ptr<Monitor>> monitors{};
        std::vector<std::unique_ptr<Joystick>> joysticks{};

        entt::sigh<void(Window*, bool)> onWindowCreate{};
        entt::sigh<void(Monitor*, bool)> onMonitorConnect{};
        entt::sigh<void(Joystick*, bool)> onJoystickConnect{};
    };
}