#pragma once
// TODO: Refactor
#if PLATFORM_ANDROID
#include "fusion/core/android.hpp"
#include "fusion/devices/window.hpp"

namespace android {
    class Window : public fe::Window {
    public:
        Window();
        ~Window() override;

        void* getNativeWindow() override { return androidApp->window; }

        bool shouldClose() override { return destroy; };
        void shouldClose(bool flag) override { if (flag) ANativeActivity_finish(androidApp->state->activity); }

        void pollEvents() override;
        void  waitEvents() override;

        int32_t onInput(AInputEvent* event);

        virtual std::vector<const char*> getRequiredInstanceExtensions() override;
        virtual VkResult createSurface(const VkInstance& instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) override;

    private:
        bool focused;
        bool destroy;

        static Window Instance;
    };
}
#endif