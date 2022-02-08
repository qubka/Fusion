#pragma once

#include "Base.hpp"
#include "Window.hpp"
#include "LayerStack.hpp"

#include "Fusion/ImGui/ImGuiLayer.hpp"

namespace Fusion {
    class Layer;

    class FUSION_API Application {
    public:
        Application();
        virtual ~Application();

        void run();

        void pushLayer(Layer& layer);
        void pushOverlay(Layer& overlay);

        Window& getWindow() { return window; }
        ImGuiLayer& getGuiLayer() { return imGuiLayer; }
        uint64_t getFrameCounter() const { return frameCounter; }

        static Application& Instance() { return *instance; }
    private:
        Window window{"Fusion", 1280, 720, true};
        LayerStack layers{};
        ImGuiLayer imGuiLayer{};
        uint64_t frameCounter;

        static Application* instance;
    };

    Application* CreateApplication();
}
