#pragma once

namespace fe {
    class EventQueue;
    class KeyInput;
    class MouseInput;

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

        int getWidth() const { return width; };
        int getHeight() const { return height; };
        bool isMinimized() const { return minimize; };
        void setMinimized(bool flag) { minimize = flag; };

        float getAspect() const { return static_cast<float>(getWidth()) / static_cast<float>(getHeight()); }
        glm::uvec2 getSize() const { return {getWidth(), getHeight()}; }
#if defined(VULKAN_HPP)
        glm::vec4 getViewport() const { return { 0, 0, getWidth(), getHeight() }; }
        vk::Extent2D getExtent() const { return {static_cast<uint32_t>(getWidth()), static_cast<uint32_t>(getHeight())}; }
#else
        glm::vec4 Window::getViewport() const { return { 0, getHeight(), getWidth(), -getHeight() }; } // vertical flip is required
#endif
        virtual void* getNativeWindow() = 0;

        virtual bool shouldClose() = 0;
        virtual void shouldClose(bool flag) = 0;

        virtual EventQueue& getEventQueue() = 0;
        virtual KeyInput& getKeyInput() = 0;
        virtual MouseInput& getMouseInput() = 0;

        virtual void runLoop(const std::function<void()>& frameHandler) = 0;

    protected:
        int width;
        int height;
        bool minimize;
    };
}
