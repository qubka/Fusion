#pragma once

#include "Window.hpp"
#include "LayerStack.hpp"
#include "Window.hpp"

//#include "Fusion/Renderer/Vulkan.hpp"
//#include "Fusion/Renderer/Renderer.hpp"

int main(int argc, char** argv);

namespace Fusion {
    class Layer;
    class ImGuiLayer;

    struct  CommandLineArgs {
        int count{0};
        char** args{nullptr};

        const char* operator[](int index) const {
            FE_ASSERT(index < count && "invalid argument index");
            return args[index];
        }
    };

    class Application {
    public:
        Application(std::string name = "Fusion", CommandLineArgs args = CommandLineArgs{});
        virtual ~Application() = default;
        FE_NONCOPYABLE(Application);

        void run();

        void pushLayer(Layer* layer);
        void pushOverlay(Layer* overlay);

        Window& getWindow() { return window; }
        //Vulkan& getVulkan() { return vulkan; }
        //Renderer& getRenderer() { return renderer; }

        CommandLineArgs& getCommandLineArgs() { return commandLineArgs; }
        ImGuiLayer& getImGuiLayer() { return *imGuiLayer; }
        static Application& Instance() { return *instance; }

    protected:
        Window window;
        CommandLineArgs commandLineArgs;
        //Vulkan vulkan;
        //Renderer renderer;

    private:
        LayerStack layers;
        ImGuiLayer* imGuiLayer;

        static Application* instance;
        friend int ::main(int argc, char** argv);
    };

    Application* CreateApplication(CommandLineArgs args);
}
