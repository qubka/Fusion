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

    openProject("New Project/New Project.fsproj");
}

void Sandbox::onUpdate() {
    //FS_LOG_DEBUG("{}", Time::FramesPerSecond());
}