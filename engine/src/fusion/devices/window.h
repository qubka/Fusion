#pragma once

namespace fe {
    /**
     * @brief Class used to represent a initial window information.
     */
    struct WindowInfo {
        std::string title{ "Fusion" };
        glm::uvec2 size{ 1280, 720 };
        bool isBorderless{ false };
        bool isResizable{ true };
        bool isFloating { false };
        bool isFullscreen{ false };
        bool isVSync{ false };
    };

    class Monitor;
    class Cursor;

    /**
     * @brief Class used to represent a window.
     */
    class Window {
        friend class DeviceManager;
    public:
        Window() = default;
        virtual ~Window() = default;
        NONCOPYABLE(Window);

        /**
         * Gets weather the window is borderless or not.
         * @return If the window is borderless.
         */
        virtual bool isBorderless() const = 0;

        /**
         * Sets the window to be borderless.
         * @param flag Weather or not to be borderless.
         */
        virtual void setBorderless(bool flag) = 0;

        /**
         * Gets weather the window is resizable or not.
         * @return If the window is resizable.
         */
        virtual bool isResizable() const = 0;

        /**
         * Sets the window to be resizable.
         * @param flag Weather or not to be resizable.
         */
        virtual void setResizable(bool flag) = 0;

        /**
         * Gets weather the window is floating or not, if floating the window will always display above other windows.
         * @return If the window is floating.
         */
        virtual bool isFloating() const = 0;

        /**
         * Sets the window to be floating.
         * @param flag Weather or not to be floating.
         */
        virtual void setFloating(bool flag) = 0;

        /**
         * Gets weather the window is fullscreen or not.
         * @return Fullscreen or windowed.
         */
        virtual bool isFullscreen() const = 0;

        /**
         * Sets the window to be fullscreen or windowed.
         * @param flag If the window will be fullscreen.
         * @param monitor The monitor to display in.
         */
        virtual void setFullscreen(bool flag, const Monitor* monitor) = 0;

        /**
         * Gets if the window is selected.
         * @return If the window is selected.
         */
        virtual bool isFocused() const = 0;

        /**
         * Sets the window to be focused.
         */
        virtual void setFocused() = 0;

        /**
         * Gets the windows is minimized.
         * @return If the window is minimized.
         */
        virtual bool isIconified() const = 0;

        /**
         * Sets the window to be iconified (minimized).
         * @param flag If the window will be set as iconified.
         */
        virtual void setIconified(bool flag) = 0;

        /**
         * Gets the windows is vsync.
         * @return If the window is vsync.
         */
        virtual bool isVSync() const = 0;

        /**
         * Sets the window to be vsync.
         * @param flag If the window will be set as iconified.
         */
        virtual void setVSync(bool flag) = 0;

        /**
         * Gets the windows is visible.
         * @return If the window is visible.
         */
        virtual bool isVisible() const = 0;

        /**
         * Sets the window to be visible.
         * @param flag If the window will be set as visible.
         */
        virtual void setVisible(bool flag) = 0;

        /**
         * Gets if the window should be closed.
         * @return If the window should be closed.
         */
        virtual bool isClose() const = 0;

        /**
         * Sets the window to be closed.
         * @return If the window will be set as is closed.
         */
        virtual void setClose(bool flag) = 0;

        /**
         * Gets the size of the window.
         * @param checkFullscreen If in fullscreen and true size will be the screens size.
         * @return The size.
         */
        virtual const glm::uvec2& getSize(bool checkFullscreen = true) const = 0;

        /**
         * Sets the window size.
         * @param size The new size in pixels.
         */
        virtual void setSize(const glm::ivec2& size) = 0;

        /**
         * Sets the window size limits.
         * @param minSize The min size limit in pixels.
         * @param maxSize The max size limit in pixels.
         */
        virtual void setSizeLimits(const glm::uvec2& minSize, const glm::uvec2& maxSize = {}) = 0;

        /**
         * Gets the position of the window.
         * @return The position.
         */
        virtual const glm::uvec2& getPosition() const = 0;

        /**
         * Sets the window position.
         * @param pos The new position on the screen.
         */
        virtual void setPosition(const glm::ivec2& pos) = 0;

        /**
         * Gets the title of the window.
         * @return The title.
         */
        virtual const std::string& getTitle() const = 0;

        /**
         * Sets the window title.
         * @param size The new size in pixels.
         */
        virtual void setTitle(std::string_view str) = 0;

        /**
         * Gets the contents of the clipboard as a string.
         * @return If the clipboard contents.
         */
        virtual const char* getClipboard() const = 0;

        /**
         * Sets the clipboard to the specified string.
         * @param str The content to set as the clipboard.
         */
        virtual void setClipboard(std::string_view str) = 0;

        /**
         * Gets if the window is selected.
         * @return If the window is selected.
         */
        virtual bool isSelected() const = 0;

        /**
         * If the cursor is hidden, the mouse is the display locked if true.
         * @return If the cursor is hidden.
         */
        virtual bool isCursorHidden() const = 0;

        /**
         * Sets if the operating systems cursor is hidden whilst in the display.
         * @param hidden If the system cursor should be hidden when not shown.
         */
        virtual void setCursorHidden(bool hidden) = 0;

        /**
         * Sets the cursor new style.
         * @param cursor The cursor style to use.
         */
        virtual void setCursor(const Cursor* cursor) = 0;

        /**
         * Sets window icon images.
         * @param filepaths The new icon files.
         */
        virtual void setIcons(gsl::span<const fs::path> filepaths) = 0;

        /**
         * Gets the current state of a key.
         * @param key The key to get the state of.
         * @return The keys state.
         */
        virtual InputAction getKey(Key key) const = 0;

        /**
         * Gets the current state of a mouse button.
         * @param mouseButton The mouse button to get the state of.
         * @return The mouse buttons state.
         */
        virtual InputAction getMouseButton(MouseButton button) const = 0;

        /**
         * Gets the mouses position.
         * @return The mouses position.
         */
        virtual const glm::vec2& getMousePosition() const = 0;

        /**
         * Sets the mouse position.
         * @param position The new mouse position.
         */
        virtual void setMousePosition(const glm::vec2& pos) = 0;

        /**
         * Gets the mouse position delta.
         * @return The mouse position delta.
         */
        virtual const glm::vec2& getMousePositionDelta() const = 0;

        /**
         * Gets the mouse position normalized.
         * @return The mouse position normalized.
         */
        virtual const glm::vec2& getMousePositionNorm() const = 0;

        /**
         * Gets the mouses virtual scroll position.
         * @return The mouses virtual scroll position.
         */
        virtual const glm::vec2& getMouseScroll() const = 0;

        /**
         * Sets the mouse virtual scroll position.
         * @param scroll The new mouse virtual scroll position.
         */
        virtual void setMouseScroll(const glm::vec2& scroll) = 0;

        /**
         * Gets the mouse scroll delta.
         * @return The mouse scroll delta.
         */
        virtual const glm::vec2& getMouseScrollDelta() const = 0;

        /**
         * Gets the current monitor if presents.
         * @return The monitor.
         */
        virtual const Monitor* getCurrentMonitor() const = 0;

        /**
         * Gets the current window object.
         * @return The object.
         */
        virtual void* getNativeWindow() const = 0;

        /**
         * Creates a Vulkan surface for the specified window.
         * @param instance The instance that the physical device belongs to.
         * @param allocator The allocator to use, or `NULL` to use the default
         * @param surface Where to store the handle of the surface.
         * @return `VK_SUCCESS` if successful, or a Vulkan error code if an [error](@ref error_handling) occurred.
         */
        virtual VkResult createSurface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const = 0;

    protected:
        /**
         * Run every frame as long as the window has work to do.
         */
        virtual void onUpdate() = 0;

    public:
        /**
         * Event when the window is resized.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnResize() { return entt::sink{onResize}; }

        /**
         * Event when the window framebuffer is resized.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnFramebufferResize() { return entt::sink{onFramebufferResize}; }

        /**
         * Event when the window is moved.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnPositionChange() { return entt::sink{onPositionChange}; }

        /**
         * Event when the windows title changed.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnTitleChange() { return entt::sink{onTitleChange}; }

        /**
         * Event when the window starts.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnStart() { return entt::sink{onStart}; }

        /**
         * Event when the window requests a close.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnClose() { return entt::sink{onClose}; }

        /**
         * Event when the window requests a refresh.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnRefresh() { return entt::sink{onRefresh}; }

        /**
         * Event when the window has toggled borderless on or off.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnBorderless() { return entt::sink{onBorderless}; }

        /**
         * Event when the window has toggled resizable on or off.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnResizable() { return entt::sink{onResizable}; }

        /**
         * Event when the window has toggled floating on or off.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnFloating() { return entt::sink{onFloating}; }

        /**
         * Event when the window has toggled visibility on or off.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnShow() { return entt::sink{onShow}; }

        /**
         * Event when the window is focused or unfocused.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnFocus() { return entt::sink{onFocus}; }

        /**
         * Event when the window is minimized or maximized.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnIconify() { return entt::sink{onIconify}; }

        /**
         * Event when the has gone fullscreen or windowed.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnFullscreen() { return entt::sink{onFullscreen}; }

        /**
         * Event when a mouse button changes state.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnMouseButton() { return entt::sink{onMouseButton}; }

        /** 
         * Event when a mouse button changes state to a press.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnMousePress() { return entt::sink{onMousePress}; }

        /**
         * Event when a mouse button changes state to a release.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnMouseRelease() { return entt::sink{onMouseRelease}; }

        /**
         * Event when the mouse moves.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnMouseMotion() { return entt::sink{onMouseMotion}; }

        /**
         * Event when the mouse moves.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnMouseMotionNorm() { return entt::sink{onMouseMotionNorm}; }

        /**
         * Event when the mouse enters the window.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnMouseEnter() { return entt::sink{onMouseEnter}; }

        /**
         * Event when the scroll wheel changes.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnMouseScroll() { return entt::sink{onMouseScroll}; }

        /**
         * Event when a key changes state.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnKey() { return entt::sink{onKey}; }

        /**
         * Event when a key changes state to a press.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnKeyPress() { return entt::sink{onKeyPress}; }

        /**
         * Event when a key changes state to a release.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnKeyRelease() { return entt::sink{onKeyRelease}; }

        /**
         * Event when a key changes state to a hold.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnKeyHold() { return entt::sink{onKeyHold}; }

        /**
         * Event when a character has been typed.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnCharInput() { return entt::sink{onCharInput}; }

        /**
         * Event when a group of files/folders is dropped onto the window.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnFileDrop() { return entt::sink{onFileDrop}; }

        /**
         * Event when the window content scale is changed.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnContentScale() { return entt::sink{onContentScale}; }

        /**
         * Event when the window is maximized.
         * @return A sink is used to connect listeners to signals and to disconnect them.
         */
        auto OnMaximize() { return entt::sink{onMaximize}; }

    protected:
        entt::sigh<void(const glm::uvec2 &)> onResize{};
        entt::sigh<void(const glm::uvec2 &)> onFramebufferResize{};
        entt::sigh<void(const glm::uvec2 &)> onPositionChange{};
        entt::sigh<void(const std::string &)> onTitleChange{};
        entt::sigh<void()> onStart{};
        entt::sigh<void()> onClose{};
        entt::sigh<void()> onRefresh{};
        entt::sigh<void(bool)> onBorderless{};
        entt::sigh<void(bool)> onResizable{};
        entt::sigh<void(bool)> onFloating{};
        entt::sigh<void(bool)> onShow{};
        entt::sigh<void(bool)> onFocus{};
        entt::sigh<void(bool)> onIconify{};
        entt::sigh<void(bool)> onFullscreen{};
        entt::sigh<void(MouseButton, InputAction, bitmask::bitmask<InputMod>)> onMouseButton{};
        entt::sigh<void(MouseButton)> onMousePress{};
        entt::sigh<void(MouseButton)> onMouseRelease{};
        entt::sigh<void(const glm::vec2 &)> onMouseMotion{};
        entt::sigh<void(const glm::vec2 &)> onMouseMotionNorm{};
        entt::sigh<void(bool)> onMouseEnter{};
        entt::sigh<void(const glm::vec2 &)> onMouseScroll{};
        entt::sigh<void(Key, InputAction, Key, bitmask::bitmask<InputMod>)> onKey{};
        entt::sigh<void(Key)> onKeyPress{};
        entt::sigh<void(Key)> onKeyRelease{};
        entt::sigh<void(Key)> onKeyHold{};
        entt::sigh<void(uint32_t)> onCharInput{};
        entt::sigh<void(gsl::span<const char*>)> onFileDrop{};
        entt::sigh<void(const glm::vec2 &)> onContentScale{};
        entt::sigh<void(bool)> onMaximize{};
    };
}
