#pragma once

#include "Core.hpp"
#include "LayerStack.hpp"

namespace Fusion {
    class Event;
    class Window;

    class WindowCloseEvent;

#define BIND_EVENT_FUNC(x) std::bind(&Application::x, this, std::placeholders::_1)

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

        std::unique_ptr<Window> window;
        LayerStack layerStack;
        bool running{true};
    };

    Application* CreateApplication();
}
