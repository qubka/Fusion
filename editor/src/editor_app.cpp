#include "editor_app.hpp"
#include "editor_layer.hpp"

using namespace fe;

EditorApp::EditorApp(const std::string& name, const Version& version) : App{name, version} {
}

EditorApp::~EditorApp() {

}

void EditorApp::onStart() {
    pushLayer(new EditorLayer());

    setupWindow();
    setupVulkan();
    setupRenderer();
}

void EditorApp::onUpdate() {

}
