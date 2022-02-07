#pragma once

#include "Core.hpp"

namespace Fusion {
    class Window;
    class Input;
    class Layer;
    class Event;

    class WindowCloseEvent;

    class FUSION_API Application {
    public:
        Application();
        virtual ~Application();

        void run();

        void pushLayer(Layer& layer);
        void pushOverlay(Layer& overlay);
        void onEvent(Event& event);
    private:
        bool onWindowClose(WindowCloseEvent& event);

        Window* window;
        Input* input;
        std::deque<Layer*> layers;
        bool isRunning{true};
    };

    Application* CreateApplication();
}
