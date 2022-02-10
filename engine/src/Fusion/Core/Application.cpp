#include "Application.hpp"
#include "Layer.hpp"
#include "Time.hpp"

#include "Fusion/Input/KeyInput.hpp"
#include "Fusion/Input/MouseInput.hpp"
#include "Fusion/ImGui/ImGuiLayer.hpp"

using namespace Fusion;

Application* Application::instance{nullptr};

Application::Application(std::string name, CommandLineArgs args)
    : window{std::move(name), 1280, 720, true},
      commandLineArgs{args}
{
    FS_CORE_ASSERT(!instance, "application already exists!");
    instance = this;

    KeyInput::Setup(window);
    MouseInput::Setup(window);

    pushOverlay(new ImGuiLayer());
}

Application::~Application() {
    delete camera;
}

void Application::run() {
    while (!window.shouldClose()) {
        Time::Tick();

        glfwPollEvents();

        for (auto* layer : layers) {
            layer->onUpdate();
        }

        if (!window.isMinimize()) {
            for (auto* layer: layers) {
                layer->onRender();
            }

            imGuiLayer->begin();
            for (Layer* layer: layers)
                layer->onImGui();
            imGuiLayer->end();
        }

        window.onUpdate();
    }
}

void Application::pushLayer(Layer* layer) {
    layers.pushFront(layer);
}

void Application::pushOverlay(Layer* overlay) {
    layers.pushBack(overlay);
}