#include "engine.hpp"

using namespace fe;

Engine* Engine::Instance{ nullptr };

Engine::Engine(const CommandLineArgs& args)
    : version{FUSION_VERSION_MAJOR, FUSION_VERSION_MINOR, FUSION_VERSION_PATCH}
    , deviceManager{DeviceManager::Create()}
{
    Instance = this;

    Log::Init();

    deviceManager->getWindow(0)->OnClose().connect<&Engine::requestClose>(this);

    commandLineParser.parse(args);

    LOG_INFO << "Version: " << version.to_string();
    LOG_INFO << "Git: [" << GIT_COMMIT_HASH << "]:(" << GIT_TAG << ") - " << GIT_COMMIT_SUBJECT << " on " << GIT_BRANCH << " at " << GIT_COMMIT_DATE;
    LOG_INFO << "Compiled on: " << FUSION_COMPILED_SYSTEM << " from: " << FUSION_COMPILED_GENERATOR << " with: " << FUSION_COMPILED_COMPILER;

    running = true;
}

Engine::~Engine() {
    Instance = nullptr;
}

int32_t Engine::run() {
    while (running) {
        try {
            frameNumber++;
            deltaTime.update();

            deviceManager->update();

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