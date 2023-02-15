#pragma once

#include "fusion/core/default_application.hpp"

namespace fe {
    class Sandbox : public DefaultApplication {
    public:
        explicit Sandbox(std::string_view name);
        ~Sandbox() override;

    private:
        void onStart() override;
        void onUpdate() override;
    };
}