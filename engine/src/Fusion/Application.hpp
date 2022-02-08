#pragma once

#include "Core.hpp"
#include "Window.hpp"
#include "LayerStack.hpp"

namespace Fusion {
    class Layer;
    class ImGuiLayer;

    class FUSION_API Application {
    public:
        Application();
        virtual ~Application();

        void run();

        void pushLayer(Layer& layer);
        void pushOverlay(Layer& overlay);
        void popLayer(Layer& layer);
        void popOverlay(Layer& overlay);

        Window& getWindow() { return window; }

        static Application& Instance() { return *instance; }
    private:
        Window window{"Fusion", 1280, 720, true};
        LayerStack layers;
        ImGuiLayer* imGuiLayer;

        static Application* instance;
    };

    Application* CreateApplication();
}
