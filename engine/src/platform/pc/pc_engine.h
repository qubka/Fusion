#pragma once

#include "fusion/core/engine.h"

int main(int argc, char** argv);

namespace pc {
    class FUSION_API Engine : public fe::Engine {
    protected:
        explicit Engine(fe::CommandLineArgs&& args);
        ~Engine() override = default;

        int32_t run() override;

    public:
        void* getNativeApp() const override { return (void*) this; }

        friend int ::main(int argc, char** argv);
    };
}