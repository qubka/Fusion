#include "pc_engine.h"

using namespace pc;

Engine::Engine(fe::CommandLineArgs&& args) : fe::Engine{std::move(args)} {
}

int32_t Engine::run() {
    using fe::Module;
    try {
        init();
        startup();
        running = true;
        while (running) {
            // Pre-Update
            updateStage(Module::Stage::Pre);

            // Main application and devices processing
            updateMain();

            // Update
            updateStage(Module::Stage::Normal);
            // Post-Update
            updateStage(Module::Stage::Post);
            // Render-Update
            updateStage(Module::Stage::Render);
        }
        shutdown();
    }
    catch (std::exception& e) {
        LOG_FATAL << e.what();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
