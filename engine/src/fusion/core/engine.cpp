#include "engine.hpp"

using namespace fe;

Engine* Engine::Instance{ nullptr };

Engine::Engine(const CommandLineArgs& args) : version{FUSION_VERSION_MAJOR, FUSION_VERSION_MINOR, FUSION_VERSION_PATCH} {
    Instance = this;

    Log::Init();

    commandLineParser.parse(args);

    LOG_INFO << "Version: " << version.to_string();
    LOG_INFO << "Git: " << FUSION_COMPILED_COMMIT_HASH << " on " << FUSION_COMPILED_BRANCH;
    LOG_INFO << "Compiled on: " << FUSION_COMPILED_SYSTEM << " from: " << FUSION_COMPILED_GENERATOR << " with: " << FUSION_COMPILED_COMPILER;

    running = true;
}

Engine::~Engine() {
    Instance = nullptr;
    application = nullptr;
}

int32_t Engine::run() {
    while (running) {
        try {
            frameNumber++;
            deltaTime.update();

            if (application) {
                if (!application->started) {
                    application->onStart();
                    application->started = true;
                }

                application->onUpdate(deltaTime.time);
            }
        } catch (std::exception& e) {
            LOG_FATAL << e.what();
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}