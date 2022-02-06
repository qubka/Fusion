#include <Fusion.hpp>
#include "Fusion/EntryPoint.hpp"

class Sandbox : public Fusion::Application {
public:
    Sandbox() = default;
    ~Sandbox() override {

    }
};

Fusion::Application* Fusion::CreateApplication() {
    return new Sandbox();
}