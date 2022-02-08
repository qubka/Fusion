#include <Fusion.hpp>
#include "Fusion/Core/EntryPoint.hpp"
#include "imgui.h"

class ExampleLayer : public Fusion::Layer {
public:
    ExampleLayer() : Layer("ExampleLayer") {}

    void onUpdate() override {
        //FS_LOG_INFO("ExampleLayer::onUpdate");
    }

    void onImGui() override {
        ImGui::Begin("Settings");

        ImGui::Text("Hello World");

        ImGui::End();
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