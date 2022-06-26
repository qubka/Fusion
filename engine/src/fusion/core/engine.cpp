#include "engine.hpp"

#include "fusion/devices/devices.hpp"

using namespace fe;

Engine* Engine::Instance = nullptr;

Engine::Engine(const CommandLineArgs& args) : version{FUSION_VERSION_MAJOR, FUSION_VERSION_MINOR, FUSION_VERSION_PATCH} {
    Instance = this;

    Log::Init();

    LOG_INFO << "Version: " << version.to_string();
    LOG_INFO << "Git: [" << GIT_COMMIT_HASH << "]:(" << GIT_TAG << ") - " << GIT_COMMIT_SUBJECT << " on " << GIT_BRANCH << " at " << GIT_COMMIT_DATE;
    LOG_INFO << "Compiled on: " << FUSION_COMPILED_SYSTEM << " from: " << FUSION_COMPILED_GENERATOR << " with: " << FUSION_COMPILED_COMPILER;

    commandLineParser.parse(args);

    devices = Devices::Init();
    devices->getWindow(0)->OnClose().connect<&Engine::requestClose>(this);

    for (auto [type, module] : Module::Registry()) {
        modules.emplace(type, module.create());
        moduleStages[module.stage].emplace_back(type);
    }

    running = true;
}

Engine::~Engine() {
    application = nullptr;
    Instance = nullptr;
}

int32_t Engine::run() {
    while (running) {
        try {
            frameNumber++;
            deltaTime.update();

            devices->update();

            if (application) {
                if (!application->started) {
                    application->start();
                    application->started = true;
                }

                application->update(deltaTime.time);
            }


        } catch (std::exception& e) {
            LOG_FATAL << e.what();
            return EXIT_FAILURE;
        }
    }


    return EXIT_SUCCESS;
}