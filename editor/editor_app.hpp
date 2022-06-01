#include "fusion/core/application.hpp"
#include "editor_layer.hpp"

namespace fe {
    class EditorApp : public Application {
    public:
        EditorApp(const CommandLineArgs& args) : Application{"Fusion Editor", args} {
            pushLayer(new EditorLayer{*this});
        }
    };
}