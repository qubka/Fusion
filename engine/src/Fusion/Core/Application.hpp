#pragma once

#include "LayerStack.hpp"

#include "Fusion/Renderer/Window.hpp"
#include "Fusion/Renderer/Vulkan.hpp"
#include "Fusion/Renderer/Renderer.hpp"
#include "Fusion/Input/Input.hpp"


int main(int argc, char** argv);

namespace Fusion {
    class Layer;
    class ImGuiLayer;

    struct FUSION_API CommandLineArgs {
        int count{0};
        char** args{nullptr};

        const char* operator[](int index) const {
            FE_ASSERT(index < count && "invalid argument index");
            return args[index];
        }
    };

    class FUSION_API Application {
    public:
        Application(std::string name = "Fusion App", CommandLineArgs args = CommandLineArgs{});
        virtual ~Application() = default;

        void run();

        void pushLayer(Layer* layer);
        void pushOverlay(Layer* overlay);

        Window& getWindow() { return window; }
        Vulkan& getVulkan() { return vulkan; }
        Renderer& getRenderer() { return renderer; }

        CommandLineArgs& getCommandLineArgs() { return commandLineArgs; }
        ImGuiLayer& getImGuiLayer() { return *imGuiLayer; }
        static Application& Instance() { return *instance; }
    private:
        Window window;
        CommandLineArgs commandLineArgs;
        Vulkan vulkan;
        Renderer renderer;

        LayerStack layers;
        ImGuiLayer* imGuiLayer;
        //std::vector<Input*> inputs;

        static Application* instance;
        friend int ::main(int argc, char** argv);
    };

    Application* CreateApplication(CommandLineArgs args);
}
