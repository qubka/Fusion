#include "editor_app.hpp"
#include "fusion/core/entry_point.hpp"


fe::Application* fe::CreateApplication(fe::CommandLineArgs args) {
    return new fe::EditorApp{args};
}
