#pragma once

#include "fusion/core/engine.h"

namespace fe::pc {
    class FUSION_API Engine : public fe::Engine {
    public:
        explicit Engine(CommandLineArgs&& args);
        ~Engine() override = default;

        int32_t run() override;

        void* getNativeApp() const override { return (void*) this; }
    };
}