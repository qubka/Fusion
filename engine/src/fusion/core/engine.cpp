#include "engine.h"
#include "module.h"
#include "time.h"

#include "fusion/devices/device_manager.h"
#include "fusion/graphics/graphics.h"
#include "fusion/filesystem/file_system.h"
#include "fusion/scene/scene_manager.h"
#include "fusion/assets/asset_registry.h"
#include "fusion/input/input.h"
#include "fusion/debug/debug_renderer.h"
#include "fusion/scripting/script_engine.h"

#include "fusion/bitmaps/gli_toolbox.h"
#include "fusion/bitmaps/stb_toolbox.h"

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
    // Destroy modules in reverse order of insertion
    for (auto it = modules.rbegin(); it != modules.rend(); ++it) {
        it->reset();
    }
    Instance = nullptr;
}

void Engine::init() {
    Time::Register("Time", Module::Stage::Pre);
    FileSystem::Register("FileSystem", Module::Stage::Never);
    Input::Register("Input", Module::Stage::Normal);
    Graphics::Register("Graphics", Module::Stage::Render);
    SceneManager::Register("SceneManager", Module::Stage::Normal);
    AssetRegistry::Register("AssetRegistry", Module::Stage::Post);
    DebugRenderer::Register("DebugRenderer", Module::Stage::Render);
    ScriptEngine::Register("FileSystem", Module::Stage::Never);

    StbToolbox::Register(".jpeg", ".jpg", ".png", ".bmp", ".hdr", ".psd", ".tga", ".gif", ".pic", ".pgm", ".ppm");
    GliToolbox::Register(".ktx", ".kmg", ".dds");

    // Create all registered modules
    for (const auto& [type, module] : Module::Registry()) {
        auto index = static_cast<uint32_t>(modules.size());
        modules.push_back(module.create());
        stages[me::enum_integer(module.stage)].push_back(index);
        LOG_DEBUG << "Module: \"" << module.name << "\" was registered for the \"" << me::enum_name(module.stage) << "\" stage";
    }
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
            devices->onUpdate();
            if (application) {
                if (!application->started) {
                    application->onStart();
                    application->started = true;
                }
                application->onUpdate();
            }

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

void Engine::startup() {
    if (application && !application->started) {
        application->onStart();
        application->started = true;
    }

    for (auto& module : modules) {
        if (!module->started) {
            module->onStart();
            module->started = true;
        }
    }
}

void Engine::shutdown() {
    if (application && application->started) {
        application->started = false;
        application->onStop();
    }

    for (auto& module : modules) {
        if (module->started) {
            module->started = false;
            module->onStop();
        }
    }
}

void Engine::updateStage(Module::Stage stage) {
    for (auto index : stages[me::enum_integer(stage)])
        modules[index]->onUpdate();
}