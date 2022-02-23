#include "editor_layer.hpp"
#include "fusion/core/application.hpp"

namespace fe {
    class EditorApp : public Application {
    public:
        EditorApp(const CommandLineArgs& args) : Application{ "Vulkan Example - Offscreen rendering", args } {
            pushLayer(new EditorLayer{ *this });
        }

        ~EditorApp() {
        }
    };
}