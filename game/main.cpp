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

class Sandbox : public Fusion::Application {
public:
    Sandbox(Fusion::CommandLineArgs args) : Application{"Sandbox", args} {
        pushLayer(new ExampleLayer());
    }
    ~Sandbox() override {
    }
};

Fusion::Application* Fusion::CreateApplication(CommandLineArgs args) {
    return new Sandbox(args);
}