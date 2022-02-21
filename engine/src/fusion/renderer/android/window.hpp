#pragma once
#if defined(ANDROID)

#include "fusion/core/android.hpp"
#include "fusion/core/window.hpp"
#include "fusion/events/event_queue.hpp"

namespace android {
    class Window : public Fusion::Window {
    public:
        Window();
        ~Window() override = default;

        void* getNativeWindow() override { return androidApp->window; }
        int getWidth() override { return width; }
        int getHeight() override { return height; }
        glm::uvec2 getSize() override { return { width, height }; }
        float getAspect() override { return aspect; }
        const std::string& getTitle() override { return ""; }
        glm::vec4 getViewport() override;
        Fusion::EventQueue& getEventQueue() override { return eventQueue; }

        bool shouldClose();
        bool shouldClose(bool flag) { if (flag) ANativeActivity_finish(androidApp->state->activity); }

        void runLoop(const std::function<void()>& frameHandler) override {
            while (!shouldClose()) {
                frameHandler();
            }
        }

        bool isFocuses() { return focused; }
        bool setFocuses(bool flag) { focused = flag; }

        int32_t onInput(AInputEvent* event);

    private:
        //ANativeWindow* window; use -> androidApp->window
        int width;
        int height;
        float aspect;
        bool focused;

        static void resetInputs();

        static Window instance;

        Fusion::EventQueue eventQueue;
    };
}
#endif