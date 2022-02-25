#pragma once
// TODO: Refactor
#if defined(ANDROID)
#include "fusion/core/android.hpp"
#include "fusion/core/window.hpp"

namespace android {
    class Window : public fe::Window {
    public:
        Window();
        ~Window() override = default;

        void* getNativeWindow() override { return androidApp->window; }

        bool shouldClose() override { return destroy; };
        void shouldClose(bool flag) override { if (flag) ANativeActivity_finish(androidApp->state->activity); }

        void pollEvents() override;

        int32_t onInput(AInputEvent* event);

    private:
        bool focused;
        bool destroy;

        static Window instance;
    };
}
#endif