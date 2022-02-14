#include <Fusion.hpp>
#include "Fusion/Core/EntryPoint.hpp"

class ExampleLayer : public Fusion::Layer {
public:
    ExampleLayer() : Layer{"ExampleLayer"} {
    }

    void onUpdate() override {
    }

    void onRender() override {

    }

    void onImGui() override {

    }
};

class SandboxApp : public Fusion::Application {
public:
    SandboxApp(Fusion::CommandLineArgs args) : Application{"Sandbox", args} {
        pushLayer(new ExampleLayer{});
    }
    ~SandboxApp() override {
    }
};

Fusion::Application* Fusion::CreateApplication(CommandLineArgs args) {
    return new SandboxApp{args};
}