#include "engine.hpp"
#include "module.hpp"
#include "time.hpp"

#include "fusion/devices/device_manager.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/filesystem/file_system.hpp"
#include "fusion/filesystem/virtual_file_system.hpp"
#include "fusion/scene/scene_manager.hpp"
#include "fusion/assets/asset_registry.hpp"
#include "fusion/input/input.hpp"

#include "fusion/bitmaps/gli_toolbox.hpp"
#include "fusion/bitmaps/stb_toolbox.hpp"

using namespace fe;

Engine* Engine::Instance = nullptr;

Engine::Engine(CommandLineArgs&& args)
        : commandLineArgs{std::move(args)}
        , version{FUSION_VERSION_VARIANT, FUSION_VERSION_MAJOR, FUSION_VERSION_MINOR, FUSION_VERSION_PATCH} {
    Instance = this;

    Log::Init();

    LOG_INFO << "Version: " << version.toString();
    LOG_INFO << "Git: [" << GIT_COMMIT_HASH << "]:(" << GIT_TAG << ") - " << GIT_COMMIT_SUBJECT << " on " << GIT_BRANCH << " at " << GIT_COMMIT_DATE;
    LOG_INFO << "Compiled on: " << FUSION_COMPILED_SYSTEM << " from: " << FUSION_COMPILED_GENERATOR << " with: " << FUSION_COMPILED_COMPILER;

    commandLineParser.parse(commandLineArgs);

    devices = DeviceManager::Init();
}

Engine::~Engine() {
    application = nullptr;
    Instance = nullptr;
}

void Engine::init() {
    Time::Register("Time", Module::Stage::Pre);
    FileSystem::Register("FileSystem", Module::Stage::Never);
    VirtualFileSystem::Register("VirtualFileSystem", Module::Stage::Never);
    Input::Register("Input", Module::Stage::Normal);
    Graphics::Register("Graphics", Module::Stage::Render);
    SceneManager::Register("SceneManager", Module::Stage::Normal);
    AssetRegistry::Register("AssetRegistry", Module::Stage::Post);

    StbToolbox::Register(".jpeg", ".jpg", ".png", ".bmp", ".hdr", ".psd", ".tga", ".gif", ".pic", ".pgm", ".ppm");
    GliToolbox::Register(".ktx", ".kmg", ".dds");

    sortModules();
}

int32_t Engine::run() {
    try {
        init();
        running = true;
        while (running) {
            // Pre-Update
            updateStage(Module::Stage::Pre);

            // Main application and devices processing
            devices->onUpdate();
            if (application) {
                startup();
                application->onUpdate();
            }

            // Update
            updateStage(Module::Stage::Normal);
            // Post-Update
            updateStage(Module::Stage::Post);
            // Render-Update
            updateStage(Module::Stage::Render);
        }
    }
    catch (std::exception& e) {
        LOG_FATAL << e.what();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void Engine::startup() {
    if (!application->started) {
        application->onStart();
        application->started = true;

        for (auto& [type, module] : modules) {
            if (!module->started) {
                module->onStart();
                module->started = true;
            }
        }
    }
}

void Engine::updateStage(Module::Stage stage) {
    for (const auto& module : stages[stage])
        modules[module]->onUpdate();
}

void Engine::sortModules() {
    // Use the table to sort the modules for each stage depending on the number of mentions
    // in the list of requirements specified in the registration of each module
    std::unordered_map<Module::Stage, std::unordered_map<type_index, uint32_t>> dependencies;

    // Create all registered modules
    for (const auto& [type, module] : Module::Registry()) {
        modules.emplace(type, module.create());
        stages[module.stage].push_back(type);
        for (const auto& require: module.requires) {
            dependencies[module.stage][require]++;
        }
        LOG_DEBUG << "Module: \"" << module.name << "\" was registered for the \"" << me::enum_name(module.stage) << "\" stage";
    }

    // Sort by dependency count
    for (auto& [stage, mods] : stages) {
        auto& deps = dependencies[stage];
        std::sort(mods.begin(), mods.end(), [&deps](const auto& a, const auto& b) {
            return deps[a] > deps[b];
        });
    }
}
