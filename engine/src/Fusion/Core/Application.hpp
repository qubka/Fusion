#pragma once

#include "Base.hpp"
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

        const Window& getWindow() { return window; }
        const ImGuiLayer& getGuiLayer() { return *imGuiLayer; }
        uint32_t getFrameCount() const { return frameCount; }

        static Application& Instance() { return *instance; }
    private:
        Window window{"Fusion", 1280, 720, true};
        LayerStack layers{};
        ImGuiLayer* imGuiLayer;
        uint32_t frameCount;

        static Application* instance;
    };

    Application* CreateApplication();
}
