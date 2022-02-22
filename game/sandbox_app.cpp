#include "fusion/core/entry_point.hpp"

class ExampleLayer : public fe::Layer {
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

class SandboxApp : public fe::Application {
public:
    SandboxApp(fe::CommandLineArgs args) : Application{"Sandbox", args} {
        pushLayer(new ExampleLayer{});
    }
    ~SandboxApp() override {
    }
};

fe::Application* fe::CreateApplication(CommandLineArgs args) {
    return new SandboxApp{args};
}