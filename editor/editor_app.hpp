#include "fusion/core/application.hpp"
#include "editor_layer.hpp"
#include "file_registry.hpp"

namespace fe {
    class EditorApp : public Application {
    public:
        EditorApp(const CommandLineArgs& args) : Application{ "Vulkan Example - Offscreen rendering", args } {
            pushLayer(new EditorLayer{ *this });
            registry.start();
        }

        ~EditorApp() {
            registry.destroy();
        }

        FileRegistry registry;
    };
}