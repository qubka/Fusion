#include "engine.hpp"
#include "module.hpp"

#include "fusion/devices/devices.hpp"
#include "fusion/graphics/graphics.hpp"

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
}

Engine::~Engine() {
    application = nullptr;
    Instance = nullptr;
}

void Engine::init() {
    Graphics::Register(Module::Stage::Render);

    // Use the table to sort the modules for each stage depending on the number of mentions
    // in the list of requirements specified in the registration of each module
    std::map<Module::Stage, std::unordered_map<std::type_index, size_t>> dependencies;

    // Create all registered modules
    for (const auto& [type, module] : Module::Registry()) {
        modules.emplace(type, module.create());
        moduleStages[module.stage].push_back(type);
        for (const auto& require: module.requires) {
            dependencies[module.stage][require]++;
        }
        LOG_DEBUG << "Module: " << std::quoted(type.name()) << " was registered for the " << std::quoted(me::enum_name(module.stage)) << " stage";
    }

    // Sort by dependency count
    for (auto& [stage, mods] : moduleStages) {
        auto& deps = dependencies[stage];
        std::sort(mods.begin(), mods.end(), [&deps](const auto& a, const auto& b) {
            return deps[a] > deps[b];
        });
    }
}

int32_t Engine::run() {
    try {
        init();
        running = true;
        while (running) {
            // Updates the delta time
            deltaTime.update();

            // Main application and devices processing
            devices->update();
            if (application) {
                if (!application->started) {
                    application->start();
                    application->started = true;
                }
                application->update(deltaTime.time);
            }

            // Pre-Update
            updateStage(Module::Stage::Pre);
            // Update.
            updateStage(Module::Stage::Normal);
            // Post-Update
            updateStage(Module::Stage::Post);

            // Render-Update
            updateStage(Module::Stage::Render);

            // Increment frame index
            frameNumber++;
        }
    } catch (std::exception& e) {
        LOG_FATAL << e.what();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void Engine::updateStage(Module::Stage stage) {
    for (const auto& moduleId : moduleStages[stage])
        modules[moduleId]->update(deltaTime.time);
}