#pragma once

#include "monitor.h"
#include "window.h"
#include "joystick.h"
#include "cursor.h"

namespace fe {
    /**
     * @brief Module used for managing some devices.
     */
    class FUSION_API DeviceManager {
        friend class Engine;
    public:
        DeviceManager();
        virtual ~DeviceManager();
        NONCOPYABLE(DeviceManager);

        static std::unique_ptr<DeviceManager> Init();

        static DeviceManager* Get() { return Instance; }

        /**
         * Waits until events are queued and processes them.
         */
        virtual void waitEvents() = 0;

        /**
         * This function returns the platform-specific scancode of the specified key.
         * @param key Any named key.
         * @return The platform-specific scancode for the key, or 255 if an error occurred.
         */
        virtual Key getScanCode(Key key) const = 0;

        /**
         * This function returns the name of the specified printable key, encoded as UTF-8.
         * @param key The key to query.
         * @param scancode The scancode of the key to query.
         * @return The UTF-8 encoded, layout-specific name of the key, or NULL.
         */
        virtual std::string getKeyName(Key key, Key scan) const = 0;

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
        virtual void updateGamepadMappings(std::string_view mappings) = 0;

        virtual Window* createWindow(const WindowInfo& windowInfo) = 0;
        void destroyWindow(size_t id);
        void destroyWindow(const Window* window);

        virtual Cursor* createCursor(const fs::path& filepath, fe::CursorHotspot hotspot) = 0;
        void destroyCursor(size_t id);
        void destroyCursor(const Cursor* cursor);

        const Window* getWindow(size_t id) const { return id < windows.size() ? windows[id].get() : nullptr; }
        Window* getWindow(size_t id) { return id < windows.size() ? windows[id].get() : nullptr; }

        const Monitor* getPrimaryMonitor();
        const Monitor* getMonitor(size_t id) const { return id < monitors.size() ? monitors[id].get() : nullptr; }
        Monitor* getMonitor(size_t id) { return id < monitors.size() ? monitors[id].get() : nullptr; }

        const Joystick* getJoystick(size_t id) const { return id < joysticks.size() ? joysticks[id].get() : nullptr; }
        Joystick* getJoystick(size_t id) { return id < joysticks.size() ? joysticks[id].get() : nullptr; }

        const Cursor* getCursor(size_t id) const { return id < cursors.size() ? cursors[id].get() : nullptr; }
        Cursor* getCursor(size_t id) { return id < cursors.size() ? cursors[id].get() : nullptr; }

        const std::vector<std::unique_ptr<Window>>& getWindows() const { return windows; };
        const std::vector<std::unique_ptr<Monitor>>& getMonitors() const { return monitors; };
        const std::vector<std::unique_ptr<Joystick>>& getJoysticks() const { return joysticks; };
        const std::vector<std::unique_ptr<Cursor>>& getCursors() const { return cursors; };

        /**
         * Gets the current manager object.
         * @return The object.
         */
        virtual void* getNativeManager() const = 0;

    protected:
        /**
         * Called when the manager is start working.
         */
        virtual void onStart() = 0;

        /**
         * Run every frame as long as the manager has work to do.
         */
        virtual void onUpdate() = 0;

        /**
         * Called when the manager is stop working.
         */
        virtual void onStop() = 0;

    public:
        /**
         * Event when a window has been created or destroy.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnWindowCreate() { return entt::sink{onWindowCreate}; }

        /**
         * Event when a cursor has been created or destroy.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnCursorCreate() { return entt::sink{onCursorCreate}; }

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
        std::vector<std::unique_ptr<Window>> windows{};
        std::vector<std::unique_ptr<Monitor>> monitors{};
        std::vector<std::unique_ptr<Joystick>> joysticks{};
        std::vector<std::unique_ptr<Cursor>> cursors{};

        entt::sigh<void(Window*, bool)> onWindowCreate{};
        entt::sigh<void(Cursor*, bool)> onCursorCreate{};
        entt::sigh<void(Monitor*, bool)> onMonitorConnect{};
        entt::sigh<void(Joystick*, bool)> onJoystickConnect{};

        bool started{ false };

        static DeviceManager* Instance;
    };
}