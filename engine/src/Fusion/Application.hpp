#pragma once

#include "Core.hpp"

namespace Fusion {
    class FUSION_API Application {
    public:
        Application() = default;
        virtual ~Application() = default;
        Application(const Application &) = delete;
        Application(Application &&) = delete;
        Application &operator=(const Application &) = delete;
        Application &operator=(Application &&) = delete;

        void run();
    private:
    };

    Application* CreateApplication();
}
