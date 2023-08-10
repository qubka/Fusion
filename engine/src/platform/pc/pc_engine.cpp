#include "pc_engine.h"

using namespace fe::pc;

Engine::Engine(CommandLineArgs&& args) : fe::Engine{std::move(args)} {
}

int32_t Engine::run() {
    try {
        init();
        startup();
        running = true;
        while (running) {
            // Pre-Update
            updateStage(Module::Stage::Pre);

            // Main application and devices processing
            updateMain();

            // Post-Update
            updateStage(Module::Stage::Post);
			
            // Render-Update
            updateStage(Module::Stage::Render);
        }
        shutdown();
    }
    catch (std::exception& e) {
        FE_LOG_FATAL(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
