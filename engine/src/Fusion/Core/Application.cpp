#include "Application.hpp"
#include "Layer.hpp"
#include "Time.hpp"

#include "Fusion/ImGui/ImGuiLayer.hpp"
#include "Fusion/Input/Input.hpp"

using namespace Fusion;

Application* Application::instance{nullptr};

Application::Application(std::string name, CommandLineArgs args)
    : commandLineArgs{args},
      window{std::move(name), 1280, 720},
      vulkan{window},
      renderer{vulkan}
{
    FE_ASSERT(!instance && "application already exists!");
    instance = this;

    //KeyInput::Setup(window);
    //MouseInput::Setup(window);
    Input::Init(window);

    imGuiLayer = new ImGuiLayer{vulkan, renderer};
    pushOverlay(imGuiLayer);
}

void Application::run() {
    while (!window.shouldClose()) {
        Time::Update();

        window.onUpdate();

        for (auto* layer : layers) {
            layer->onUpdate();
        }

        if (!window.isMinimize()) {
            if (renderer.beginFrame()) {
                auto cmd = renderer.beginOffscreenRenderPass();

                for (auto* layer: layers) {
                    layer->onRender();
                }

                renderer.endOffscreenRenderPass(cmd);

                auto commandBuffer = renderer.beginSwapChainRenderPass();

                imGuiLayer->begin();
                for (auto* layer: layers) {
                    layer->onImGui();
                }
                imGuiLayer->end(commandBuffer);

                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
        }

        Input::Update();
    }

    auto result = vulkan.getDevice().waitIdle();
    FE_ASSERT(result == vk::Result::eSuccess && "failed to wait on the device!");
}

void Application::pushLayer(Layer* layer) {
    layers.pushFront(layer);
}

void Application::pushOverlay(Layer* overlay) {
    layers.pushBack(overlay);
}