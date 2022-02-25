#pragma once

#include "fusion/input/mouse_codes.hpp"
#include "fusion/input/key_codes.hpp"
#include "fusion/input/action_codes.hpp"

#include <signals/signals.hpp>

namespace fe {
    class Window {
    public:
        Window(int width, int height)
            : width{width}
            , height{height}
            , minimize{false} {
            assert(width >= 0 && height >= 0 && "Width or height cannot be negative");
        }
        virtual ~Window() = default;
        FE_NONCOPYABLE(Window);

        int getWidth() const { return width; }
        int getHeight() const { return height; }
        bool isMinimized() const { return minimize; }
        void setMinimized(bool flag) { minimize = flag; }

        float getAspect() const { return static_cast<float>(width) / static_cast<float>(height); }
        glm::uvec2 getSize() const { return {width, height}; }
#if defined(VULKAN_HPP)
        glm::vec4 getViewport() const { return { 0, 0, width, height }; }
        vk::Extent2D getExtent() const { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
#else
        glm::vec4 getViewport() const { return { 0, height, width, -height }; } // vertical flip is required
#endif
        virtual void* getNativeWindow() = 0;

        virtual bool shouldClose() = 0;
        virtual void shouldClose(bool flag) = 0;

        virtual void pollEvents() = 0;

    protected:
        int width;
        int height;
        bool minimize;

    public:
        fteng::signal<void(const glm::ivec2 &)> PositionChangeEvent;
        fteng::signal<void(const glm::ivec2 &)> SizeChangeEvent;
        fteng::signal<void(const glm::ivec2 &)> FramebufferEvent;
        fteng::signal<void()> StartEvent;
        fteng::signal<void()> CloseEvent;
        fteng::signal<void()> RefreshEvent;
        fteng::signal<void(bool)> FocusEvent;
        fteng::signal<void(bool)> IconifyEvent;
        fteng::signal<void(MouseCode, ActionCode)> MouseButtonEvent;
        fteng::signal<void(MouseCode)> MousePressEvent;
        fteng::signal<void(MouseCode)> MouseReleaseEvent;
        fteng::signal<void(const glm::vec2 &)> MouseMotionEvent;
        fteng::signal<void(const glm::vec2 &)> MouseMotionNormEvent;
        fteng::signal<void(bool)> MouseEnterEvent;
        fteng::signal<void(const glm::vec2 &)> MouseScrollEvent;
        fteng::signal<void(KeyCode, ActionCode)> KeyEvent;
        fteng::signal<void(KeyCode)> KeyPressEvent;
        fteng::signal<void(KeyCode)> KeyReleaseEvent;
        fteng::signal<void(KeyCode)> KeyHoldEvent;
        fteng::signal<void(uint32_t)> CharInputEvent;
        fteng::signal<void(const std::vector<std::string> &)> FileDropEvent;
        fteng::signal<void(const glm::vec2 &)> ContentScaleEvent;
        fteng::signal<void(bool)> MaximizeEvent;
        fteng::signal<void(bool)> MonitorEvent;
        fteng::signal<void(uint8_t, bool)> JoystickEvent;
    };
}
