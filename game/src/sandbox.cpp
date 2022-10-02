#include "sandbox.hpp"
#include "sandbox_renderer.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/graphics/graphics.hpp"

using namespace fe;

Sandbox::Sandbox(std::string_view name) : DefaultApplication{name} {

}

Sandbox::~Sandbox() {

}

void Sandbox::onStart() {
    DefaultApplication::onStart();

    Graphics::Get()->setRenderer(std::make_unique<SandboxRenderer>());

    openProject("New Project/New Project.fsproj");
}

void Sandbox::onUpdate() {
    LOG_DEBUG << Time::FramesPerSecond();
}