#pragma once

#include "fusion/devices/window.h"
#include "fusion/devices/monitor.h"

extern "C" {
    typedef struct GLFWwindow GLFWwindow;
}

namespace fe::glfw {
    class FUSION_API Window final : public fe::Window {
        friend class DeviceManager;
    public:
        Window(const VideoMode& videoMode, const WindowInfo& windowInfo);
        ~Window() override;

        bool isBorderless() const override { return borderless; }
        void setBorderless(bool flag) override;

        bool isResizable() const override { return resizable; }
        void setResizable(bool flag) override;

        bool isFloating() const override { return floating; }
        void setFloating(bool flag) override;

        bool isFullscreen() const override { return fullscreen; }
        void setFullscreen(bool flag, const fe::Monitor* monitor = nullptr) override;

        bool isFocused() const override { return focused; }
        void setFocused() override;

        bool isIconified() const override { return iconified; }
        void setIconified(bool flag) override;

        bool isVSync() const override { return vsync; }
        void setVSync(bool flag) override { vsync = flag; }

        bool isVisible() const override { return visible; }
        void setVisible(bool flag) override;

        bool isClose() const override;
        void setClose(bool flag) override;

        const glm::uvec2& getSize(bool checkFullscreen = true) const override { return (fullscreen && checkFullscreen) ? fullscreenSize : size; }
        void setSize(const glm::ivec2& size) override;
        void setSizeLimits(const glm::uvec2& minSize, const glm::uvec2& maxSize) override;

        const glm::uvec2& getPosition() const override { return position; }
        void setPosition(const glm::ivec2& pos) override;

        const std::string& getTitle() const override { return title; }
        void setTitle(std::string_view str) override;

        const char* getClipboard() const override;
        void setClipboard(std::string_view str) override;

        bool isSelected() const override { return selected; }

        bool isCursorHidden() const override { return cursorHidden; }
        void setCursorHidden(bool hidden) override;

        void setCursor(const Cursor* cursor) override;
        void setIcons(gsl::span<const fs::path> filepaths) override;

        InputAction getKey(Key key) const override;
        InputAction getMouseButton(MouseButton button) const override;

        const glm::vec2& getMousePosition() const override { return mousePosition; }
        const glm::vec2& getMousePositionDelta() const override { return mousePositionDelta; }
        const glm::vec2& getMousePositionNorm() const override { return mousePositionNorm; }
        void setMousePosition(const glm::vec2& pos) override;

        const glm::vec2& getMouseScroll() const override { return mouseScroll; }
        void setMouseScroll(const glm::vec2& scroll) override;
        const glm::vec2& getMouseScrollDelta() const override { return mouseScrollDelta; }

        const Monitor* getCurrentMonitor() const override;
		
        void* getNativeWindow() const override { return window; }
		void setNativeWindow(void* ptr) override {};

        VkResult createSurface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const override;

    protected:
        void onUpdate() override;

    private:
        GLFWwindow* window{ nullptr };

        glm::uvec2 size;
        glm::uvec2 fullscreenSize;
        glm::uvec2 position;
        std::string title;

        bool borderless{ false };
        bool resizable{ false };
        bool floating{ false };
        bool fullscreen{ false };
        bool iconified{ false };
        bool vsync{ false };
        bool focused{ true };
        bool visible{ true };
        bool selected{ true };
        bool cursorHidden{ false };

        glm::vec2 mouseLastPosition;
        glm::vec2 mousePosition;
        glm::vec2 mousePositionDelta;
        glm::vec2 mousePositionNorm;
        glm::vec2 mouseLastScroll;
        glm::vec2 mouseScroll;
        glm::vec2 mouseScrollDelta;

        /// Workaround for C++ class using a c-style-callback
        static void PosCallback(GLFWwindow* window, int x, int y);
        static void SizeCallback(GLFWwindow* window, int width, int height);
        static void CloseCallback(GLFWwindow* window);
        static void RefreshCallback(GLFWwindow* window);
        static void FocusCallback(GLFWwindow* window, int focused);
        static void IconifyCallback(GLFWwindow* window, int iconified);
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void CursorPosCallback(GLFWwindow* window, double posX, double posY);
        static void CursorEnterCallback(GLFWwindow* window, int entered);
        static void ScrollCallback(GLFWwindow* window, double offsetX, double offsetY);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void CharCallback(GLFWwindow* window, unsigned int keycode);
        static void FileDropCallback(GLFWwindow* window, int count, const char** paths);
        static void MaximizeCallback(GLFWwindow* window, int maximized);
        static void ContentScaleCallback(GLFWwindow* window, float scaleX, float scaleY);
    };
}