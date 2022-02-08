#include "Application.hpp"
#include "Log.hpp"
#include "Window.hpp"
#include "Layer.hpp"

#include "Fusion/Input/KeyInput.hpp"
#include "Fusion/Input/MouseInput.hpp"

using namespace Fusion;

Application* Application::instance{nullptr};

Application::Application() {
    FS_CORE_ASSERT(!instance, "application already exists!");
    instance = this;

    KeyInput::SetupKeyInputs(window);
    MouseInput::SetupMouseInputs(window);

    pushOverlay(imGuiLayer);
}

Application::~Application() {

}

void Application::run() {
    while (!window.shouldClose()) {
        frameCounter++;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto* layer : layers) {
            layer->onUpdate();
        }

        imGuiLayer.begin();
        for (Layer* layer : layers)
            layer->onImGui();
        imGuiLayer.end();

        window.onUpdate();
    }
}

void Application::pushLayer(Layer& layer) {
    layers.pushFront(layer);
}

void Application::pushOverlay(Layer& overlay) {
    layers.pushBack(overlay);
}