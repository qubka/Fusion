#include <Fusion.hpp>
#include "Fusion/Core/EntryPoint.hpp"

using namespace Fusion;

class ExampleLayer : public Layer {
public:
    ExampleLayer() : Layer{"ExampleLayer"} {
    }

    void onUpdate() override {
    }

    void onRender() override {
    }

    void onImGui() override {

    }

    //MouseInput mouseInput{{Mouse::ButtonLeft, Mouse::ButtonRight, Mouse::ButtonMiddle}};
    //KeyInput keyInput{{Key::Escape}};
};

class Sandbox : public Application {
public:
    Sandbox(CommandLineArgs args) : Application{"Sandbox", args} {
        pushLayer(new ExampleLayer());
    }
    ~Sandbox() override {

    }
};

Fusion::Application* Fusion::CreateApplication(CommandLineArgs args) {
    return new Sandbox(args);
}