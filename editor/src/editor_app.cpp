#include "editor_app.hpp"
#include "editor_layer.hpp"
#include "main_renderer.hpp"

using namespace fe;

EditorApp::EditorApp(const std::string& name, const Version& version) : App{name, version} {
    auto graphics = std::make_unique<Graphics>();
    graphics->setRenderer(std::make_unique<MainRenderer>());
    setGraphics(std::move(graphics));
}

EditorApp::~EditorApp() {

}

void EditorApp::onStart() {
    pushLayer(new EditorLayer());
}

void EditorApp::onUpdate() {
    graphics->update();
}
