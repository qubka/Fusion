#include "editor_app.hpp"
#include "editor_layer.hpp"
#include "main_renderer.hpp"

using namespace fe;

EditorApp::EditorApp(const std::string& name, const Version& version) : App{name, version} {
}

EditorApp::~EditorApp() {

}

void EditorApp::onStart() {
    auto graphics = std::make_unique<Graphics>();
    graphics->setRenderer(std::make_unique<MainRenderer>());
    setGraphics(std::move(graphics));
}

void EditorApp::onUpdate(const Time& dt) {
    graphics->update();
}
