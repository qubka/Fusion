#pragma once

#include "fusion/core/engine.h"

struct android_app;

void android_main(struct android_app* state);

namespace android {
    class FUSION_API Engine : public fe::Engine {
    protected:
        explicit Engine(struct android_app* app, fe::CommandLineArgs&& args);
        ~Engine() override = default;

        int32_t run() override;

    public:
        void* getNativeApp() const override { return app; }

    private:
        struct android_app* app{ nullptr };
        bool render{ false };

        static void OnAppCmd(struct android_app* app, int32_t cmd);

        friend void ::android_main(struct android_app* state);
    };
}