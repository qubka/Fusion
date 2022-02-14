#include <Fusion.hpp>
#include "Fusion/Core/EntryPoint.hpp"
#include "EditorLayer.hpp"

class EditorApp : public Fusion::Application {
public:
    EditorApp(Fusion::CommandLineArgs args) : Application{"Editor", args} {
        pushLayer(new Fusion::EditorLayer{});
    }
    ~EditorApp() override {
    }
};

Fusion::Application* Fusion::CreateApplication(Fusion::CommandLineArgs args) {
    return new EditorApp(args);
}