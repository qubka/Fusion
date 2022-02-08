#pragma once

#include "Core.hpp"

namespace Fusion {
    class Window;
    class Input;
    class Layer;

    class FUSION_API Application {
    public:
        Application();
        virtual ~Application();

        void run();

        void pushLayer(Layer& layer);
        void pushOverlay(Layer& overlay);
        void addInput(Input& input);
    private:

        Window* window;
        std::deque<Layer*> layers;
        std::vector<Input*> inputs;
    };

    Application* CreateApplication();
}
