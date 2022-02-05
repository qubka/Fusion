#pragma once

#include "Core.hpp"

namespace Fusion {
    class FUSION_API Application {
    public:
        Application() = default;
        virtual ~Application() = default;

        void run();
    private:
    };

    Application* CreateApplication();
}
