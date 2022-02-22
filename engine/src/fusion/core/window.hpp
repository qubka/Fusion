#pragma once

namespace Fusion {
    class EventQueue;
    class Window {
    public:
        Window() = default;
        virtual ~Window() = default;
        FE_NONCOPYABLE(Window);

        virtual void* getNativeWindow() = 0;
        virtual int getWidth() = 0;
        virtual int getHeight() = 0;
        virtual glm::uvec2 getSize() = 0;
        virtual float getAspect() = 0;
        virtual const std::string& getTitle() = 0;
        virtual glm::vec4 getViewport() = 0;
        virtual EventQueue& getEventQueue() = 0;

        virtual bool isMinimized() = 0;
        virtual void setMinimized(bool flag) = 0;

        virtual bool shouldClose() = 0;
        virtual void shouldClose(bool flag) = 0;

        virtual void runLoop(const std::function<void()>& frameHandler) = 0;
    };
}
