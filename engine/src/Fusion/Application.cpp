#include "Application.hpp"
#include "Log.hpp"
#include "Window.hpp"
#include "Layer.hpp"
#include "Events/Event.hpp"
#include "Events/ApplicationEvents.hpp"

using namespace Fusion;

Application::Application() {
    window = std::unique_ptr<Window>(Window::create());
    window->setEventCallback(BIND_EVENT_FUNC(onEvent));
}

Application::~Application() {

}

void Application::run() {
    while (running) {
        window->onUpdate();

        for (auto* layer : layerStack) {
            layer->onUpdate();
        }
    }
}

void Application::pushLayer(Layer& layer) {
    layerStack.popLayer(layer);
}

void Application::pushOverlay(Layer& overlay) {
    layerStack.pushOverlay(overlay);
}

void Application::onEvent(Event& event) {
    FS_LOG_CORE_INFO("{0}", event.toString());

    EventDispatcher dispatcher{event};
    dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(onWindowClose));

    for (auto it = layerStack.end(); it != layerStack.begin(); ) {
        (*--it)->onEvent(event);
        if (event.isHandled())
            break;
    }
}

bool Application::onWindowClose(WindowCloseEvent& event) {
    running = false;
    return true;
}