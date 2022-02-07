#include <Fusion.hpp>
#include "Fusion/EntryPoint.hpp"

class ExampleLayer : public Fusion::Layer {
public:
    ExampleLayer() : Layer("ExampleLayer") {}

    void onUpdate() override {
        //FS_LOG_INFO("ExampleLayer::onUpdate");
    }

    void onEvent(Fusion::Event& event) override {
        FS_LOG_INFO("{0}", event.toString());
    }
};

class Sandbox : public Fusion::Application {
public:
    Sandbox() {
        pushLayer(*new ExampleLayer());
    }
    ~Sandbox() override {

    }
};

Fusion::Application* Fusion::CreateApplication() {
    return new Sandbox();
}