#pragma once
// TODO: Refactor
#if defined(ANDROID)
#include "fusion/core/android.hpp"
#include "fusion/core/window.hpp"
#include "fusion/events/event_queue.hpp"
#include "fusion/input/key_input.hpp"
#include "fusion/input/mouse_input.hpp"

namespace android {
    class Window : public fe::Window {
    public:
        Window();
        ~Window() override = default;

        void* getNativeWindow() override { return androidApp->window; }
        int getWidth() override { return width; }
        int getHeight() override { return height; }
        glm::uvec2 getSize() override { return { width, height }; }
        float getAspect() override { return aspect; }
        const std::string& getTitle() override { return title; }
        glm::vec4 getViewport() override;

        bool isMinimized() override { return focused; }
        void setMinimized(bool flag) override { focused = flag; }

        bool shouldClose() override;
        void shouldClose(bool flag) override { if (flag) ANativeActivity_finish(androidApp->state->activity); }

        fe::EventQueue& getEventQueue() { return eventQueue; }
        fe::KeyInput& getKeyInput() { return keyInput; }
        fe::MouseInput& getMouseInput() { return mouseInput; }

        void runLoop(const std::function<void()>& frameHandler) override {
            while (!shouldClose()) {
                frameHandler();
            }
        }

        int32_t onInput(AInputEvent* event);

    private:
        //ANativeWindow* window;
        int width;
        int height;
        float aspect;
        bool focused;
        std::string title;

        fe::EventQueue eventQueue;
        fe::KeyInput keyInput;
        fe::MouseInput mouseInput;

        static void resetInputs();

        static Window instance;
    };
}
#endif