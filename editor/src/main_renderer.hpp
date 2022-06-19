#pragma once

#include "fusion/renderer/renderer.hpp"

namespace fe {
    class MainRenderer : public Renderer {
    public:
        MainRenderer() = default;
        ~MainRenderer() override= default;

        void onStart() override {}
        void onUpdate() override {}

    private:
    };
}
