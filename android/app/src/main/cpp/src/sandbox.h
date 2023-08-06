#pragma once

#include "fusion/core/default_application.h"

namespace fe {
    class FUSION_API Sandbox : public DefaultApplication {
    public:
        explicit Sandbox(std::string_view name);
        ~Sandbox() override;

    private:
        void onStart() override;
        void onUpdate() override;
    };
}