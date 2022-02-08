#include "Application.hpp"
#include "Log.hpp"
#include "Window.hpp"
#include "Input/KeyInput.hpp"
#include "Input/MouseInput.hpp"
#include "Layer.hpp"

#include "Fusion/ImGui/ImGuiLayer.hpp"

using namespace Fusion;


Application* Application::instance{nullptr};

Application::Application() {
    FS_CORE_ASSERT(!instance, "application already exists!");
    instance = this;

    KeyInput::SetupKeyInputs(window);
    MouseInput::SetupMouseInputs(window);

    imGuiLayer = new ImGuiLayer();
    pushOverlay(*imGuiLayer);
}

Application::~Application() {

}

void Application::run() {
    while (!window.shouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto* layer : layers) {
            layer->onUpdate();
        }

        imGuiLayer->begin();
        for (Layer* layer : layers)
            layer->onImGui();
        imGuiLayer->end();

        window.onUpdate();
    }
}

void Application::pushLayer(Layer& layer) {
    layers.pushLayer(layer);
}

void Application::pushOverlay(Layer& overlay) {
    layers.pushOverlay(overlay);
}

void Application::popLayer(Layer& layer) {
    layers.popLayer(layer);
}

void Application::popOverlay(Layer& overlay) {
    layers.pushLayer(overlay);
}