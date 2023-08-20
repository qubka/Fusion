#pragma once

#include "fusion/core/engine.h"

struct android_app;

namespace fe::android {
    class FUSION_API Engine final : public fe::Engine {
    public:
        Engine(struct android_app* app, CommandLineArgs&& args);
        ~Engine() override = default;

        int32_t run() override;

        void* getNativeApp() const override { return app; }

    private:
        struct android_app* app{ nullptr };
        bool render{ false };

        static void OnAppCmd(struct android_app* app, int32_t cmd);
    };
}