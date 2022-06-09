#pragma once

#include "fusion/input/mouse_codes.hpp"
#include "fusion/input/key_codes.hpp"
#include "fusion/input/action_codes.hpp"

#include <entt/signal/sigh.hpp>

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
        virtual void waitEvents() = 0;

    protected:
        int width;
        int height;
        bool minimize;

    public:
        entt::sigh<void(const glm::ivec2 &)> PositionChangeSignal{};
        entt::sigh<void(const glm::ivec2 &)> SizeChangeSignal{};
        entt::sigh<void(const glm::ivec2 &)> FramebufferSignal{};
        entt::sigh<void()> StartSignal{};
        entt::sigh<void()> CloseSignal{};
        entt::sigh<void()> RefreshSignal{};
        entt::sigh<void(bool)> FocusSignal{};
        entt::sigh<void(bool)> IconifySignal{};
        entt::sigh<void(MouseData)> MouseButtonSignal{};
        entt::sigh<void(MouseCode)> MousePressSignal{};
        entt::sigh<void(MouseCode)> MouseReleaseSignal{};
        entt::sigh<void(const glm::vec2 &)> MouseMotionSignal{};
        entt::sigh<void(const glm::vec2 &)> MouseMotionNormSignal{};
        entt::sigh<void(bool)> MouseEnterSignal{};
        entt::sigh<void(const glm::vec2 &)> MouseScrollSignal{};
        entt::sigh<void(KeyData)> KeySignal{};
        entt::sigh<void(KeyCode)> KeyPressSignal{};
        entt::sigh<void(KeyCode)> KeyReleaseSignal{};
        entt::sigh<void(KeyCode)> KeyHoldSignal{};
        entt::sigh<void(uint32_t)> CharInputSignal{};
        entt::sigh<void(const std::vector<std::string> &)> FileDropSignal{};
        entt::sigh<void(const glm::vec2 &)> ContentScaleSignal{};
        entt::sigh<void(bool)> MaximizeSignal{};
        entt::sigh<void(bool)> MonitorSignal{};
        entt::sigh<void(uint8_t, bool)> JoystickSignal{};
    };
}
