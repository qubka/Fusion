#include "sandbox.h"
#include "sandbox_renderer.h"

#include "fusion/core/engine.h"
#include "fusion/graphics/graphics.h"

using namespace fe;

Sandbox::Sandbox(std::string_view name) : DefaultApplication{name} {

}

Sandbox::~Sandbox() {

}

void Sandbox::onStart() {
    DefaultApplication::onStart();

    Graphics::Get()->setRenderer(std::make_unique<SandboxRenderer>());

    openProject("New Project.fsproj");
}

void Sandbox::onUpdate() {
    //FE_LOG_DEBUG("{}", Time::FramesPerSecond());
}