#pragma once

#include "fusion/devices/window.h"
#include "fusion/devices/monitor.h"

class ANativeWindow;

namespace fe::android {
    class FUSION_API Window : public fe::Window {
        friend class DeviceManager;
    public:
        Window(const WindowInfo& windowInfo);
        ~Window() override;

        bool isBorderless() const override { return true; }
        void setBorderless(bool flag) override {};

        bool isResizable() const override { return false; }
        void setResizable(bool flag) override {};

        bool isFloating() const override { return false; }
        void setFloating(bool flag) override {};

        bool isFullscreen() const override { return true; }
        void setFullscreen(bool flag, const fe::Monitor* monitor = nullptr) override {};

        bool isFocused() const override { return true; }
        void setFocused() override {};

        bool isIconified() const override { return window == nullptr; }
        void setIconified(bool flag) override {};

        bool isVSync() const override { return true; }
        void setVSync(bool flag) override {}

        bool isVisible() const override { return true; }
        void setVisible(bool flag) override {};

        bool isClose() const override { return false; };
        void setClose(bool flag) override {}

        const glm::uvec2& getSize(bool checkFullscreen = true) const override { return size; }
        void setSize(const glm::ivec2& size) override {};
        void setSizeLimits(const glm::uvec2& minSize, const glm::uvec2& maxSize) override {};

        const glm::uvec2& getPosition() const override { return position; }
        void setPosition(const glm::ivec2& pos) override {};

        const std::string& getTitle() const override { return title; }
        void setTitle(std::string_view str) override { title = str; }

        const char* getClipboard() const override { return ""; }
        void setClipboard(std::string_view str) override {}

        bool isSelected() const override { return true; }

        bool isCursorHidden() const override { return true; }
        void setCursorHidden(bool hidden) override {};

        void setCursor(const Cursor* cursor) override {};
        void setIcons(gsl::span<const fs::path> filepaths) override {};

        InputAction getKey(Key key) const override { return static_cast<InputAction>(-1);};
        InputAction getMouseButton(MouseButton button) const override { return static_cast<InputAction>(-1);};

        const glm::vec2& getMousePosition() const override { return mousePosition; }
        const glm::vec2& getMousePositionDelta() const override { return mousePositionDelta; }
        const glm::vec2& getMousePositionNorm() const override { return mousePositionNorm; }
        void setMousePosition(const glm::vec2& pos) override {}

        const glm::vec2& getMouseScroll() const override { return mouseScroll; }
        void setMouseScroll(const glm::vec2& scroll) override { }
        const glm::vec2& getMouseScrollDelta() const override { return mouseScrollDelta; }

        const Monitor* getCurrentMonitor() const override { return nullptr; };
		
        void* getNativeWindow() const override { return window; }
		void setNativeWindow(void* ptr) override;

        VkResult createSurface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const override;

    protected:
        void onUpdate() override;
        //int32_t onInput(AInputEvent* event);

    private:
        ANativeWindow* window{ nullptr };

        glm::uvec2 size;
        glm::uvec2 position;
        std::string title;

        glm::vec2 mouseLastPosition;
        glm::vec2 mousePosition;
        glm::vec2 mousePositionDelta;
        glm::vec2 mousePositionNorm;
        glm::vec2 mouseLastScroll;
        glm::vec2 mouseScroll;
        glm::vec2 mouseScrollDelta;

        //void cursorPosCallback(AInputEvent* event);
    };
}