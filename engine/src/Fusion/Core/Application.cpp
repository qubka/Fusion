#include "Application.hpp"
#include "Layer.hpp"
#include "Time.hpp"

#include "Fusion/Input/KeyInput.hpp"
#include "Fusion/Input/MouseInput.hpp"

#include "Fusion/ImGui/ImGuiLayer.hpp"
#include "Fusion/Editor/EditorLayer.hpp"

using namespace Fusion;

Application* Application::instance{nullptr};

Application::Application(std::string name, CommandLineArgs args)
    : commandLineArgs{args},
      window{std::move(name), 1280, 720},
      vulkan{window},
      renderer{vulkan}
{
    FS_CORE_ASSERT(!instance, "application already exists!");
    instance = this;

    KeyInput::Setup(window);
    MouseInput::Setup(window);

    imGuiLayer = new ImGuiLayer(renderer);
    pushOverlay(imGuiLayer);
    pushOverlay(new EditorLayer());
}

void Application::run() {
    while (!window.shouldClose()) {
        Time::Tick();
        window.onUpdate();

        for (auto* layer : layers) {
            layer->onUpdate();
        }

        if (!window.isMinimize()) {
            if (auto commandBuffer = renderer.beginFrame()) {
                renderer.beginSwapChainRenderPass(commandBuffer);

                for (auto* layer: layers) {
                    layer->onRender();
                }

                imGuiLayer->begin();
                for (auto* layer: layers) {
                    layer->onImGui();
                }
                imGuiLayer->end(commandBuffer);

                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame(commandBuffer);
            }
        }

        auto result = vulkan.getDevice().waitIdle();
        FS_CORE_ASSERT(result == vk::Result::eSuccess, "failed to wait on the device!");
    }
}

void Application::pushLayer(Layer* layer) {
    layers.pushFront(layer);
}

void Application::pushOverlay(Layer* overlay) {
    layers.pushBack(overlay);
}