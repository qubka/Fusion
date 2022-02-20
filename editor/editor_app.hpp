#include "fusion/core/application.hpp"
#include "fusion/core/entry_point.hpp"
#include "editor_layer.hpp"

class EditorApp : public Fusion::Application {
public:
    EditorApp(Fusion::CommandLineArgs args) : Application{"Editor", args} {
        //pushLayer(new Fusion::EditorLayer{});
    }
    ~EditorApp() override {
    }
};