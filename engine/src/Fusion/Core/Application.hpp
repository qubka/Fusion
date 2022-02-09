#pragma once

#include "Base.hpp"
#include "Window.hpp"
#include "Log.hpp"
#include "LayerStack.hpp"

int main(int argc, char** argv);

namespace Fusion {
    class Layer;
    class ImGuiLayer;

    struct FUSION_API CommandLineArgs {
        int count{0};
        char** args{nullptr};

        const char* operator[](int index) const {
            FS_CORE_ASSERT(index < count, "invalid argument index");
            return args[index];
        }
    };

    class FUSION_API Application {
    public:
        Application(std::string name = "Fusion App", CommandLineArgs args = CommandLineArgs{});
        virtual ~Application();

        void run();

        void pushLayer(Layer* layer);
        void pushOverlay(Layer* overlay);

        Window& getWindow() { return window; }
        CommandLineArgs& getCommandLineArgs() { return commandLineArgs; }
        ImGuiLayer& getGuiLayer() { return *imGuiLayer; }

        static Application& Instance() { return *instance; }
    private:
        Window window;
        CommandLineArgs commandLineArgs;
        LayerStack layers;

        ImGuiLayer* imGuiLayer;

        static Application* instance;
        friend int ::main(int argc, char** argv);
    };

    Application* CreateApplication(CommandLineArgs args);
}
