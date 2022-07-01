#include "editor_app.hpp"
#include "editor_layer.hpp"
#include "main_renderer.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

EditorApp::EditorApp(const std::string& name, const Version& version) : App{name, version} {
}

EditorApp::~EditorApp() {

}

void EditorApp::start() {
    Graphics::Get()->setRenderer(std::make_unique<MainRenderer>());
}

void EditorApp::update() {
}
