#include "engine.h"
#include "module.h"
#include "time.h"

#include "fusion/devices/device_manager.h"

#include "fusion/bitmaps/gli_bitmap.h"
#include "fusion/bitmaps/stb_bitmap.h"

using namespace fe;

Engine* Engine::Instance = nullptr;

Engine::Engine(CommandLineArgs&& args)
        : commandLineArgs{std::move(args)}
        , version{FUSION_VERSION_VARIANT, FUSION_VERSION_MAJOR, FUSION_VERSION_MINOR, FUSION_VERSION_PATCH} {
    Instance = this;

    logger = Log::Init();

    FE_LOG_INFO("Version: {}", version.toString());
    FE_LOG_INFO("Git: [{}]:({}) - {} on {} at '{}'", GIT_COMMIT_HASH, GIT_TAG, GIT_COMMIT_SUBJECT, GIT_BRANCH, GIT_COMMIT_DATE);
    FE_LOG_INFO("Compiled on: {} from: {} with: '{}'", FUSION_COMPILED_SYSTEM, FUSION_COMPILED_GENERATOR, FUSION_COMPILED_COMPILER);

    commandLineParser.parse(commandLineArgs);

    devices = DeviceManager::Init();
}

Engine::~Engine() {
    application.reset();
    moduleHolder.reset();
    devices.reset();
    logger.reset();
    Instance = nullptr;
}

void Engine::init() {
    StbBitmap::Register(".jpeg", ".jpg", ".png", ".bmp", ".hdr", ".psd", ".tga", ".gif", ".pic", ".pgm", ".ppm");
    GliBitmap::Register(".ktx", ".kmg", ".dds");

    moduleHolder = std::make_unique<ModuleHolder>();
}

void Engine::startup() {
    if (application && !application->started) {
        application->onStart();
        application->started = true;
    }

    moduleHolder->startModules();

    if (!devices->started) {
        devices->onStart();
        devices->started = true;
    }

    started = true;
}

void Engine::shutdown() {
	running = false;

    if (devices->started) {
        devices->started = false;
        devices->onStop();
    }

    moduleHolder->stopModules();

    if (application && application->started) {
        application->started = false;
        application->onStop();
    }

    started = false;
}

void Engine::updateMain() {
    moduleHolder->updateModules<ModuleBase::Stage::Pre>();

    devices->onUpdate();
    if (application) {
        if (!application->started) {
            application->onStart();
            application->started = true;
        }
        application->onUpdate();
    }

    moduleHolder->updateModules<ModuleBase::Stage::Post>();

    moduleHolder->updateModules<ModuleBase::Stage::Render>();
}