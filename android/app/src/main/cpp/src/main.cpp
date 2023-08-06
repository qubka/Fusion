#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/log.h>

#include "platform/android/android_engine.h"

#include "sandbox.h"

#define LOG_TAG "game-android"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

void android_main(struct android_app* state) {
    using namespace fe;

    LOGI("Started!");

    // Creates the engine
    android::Engine engine{state, {}};

    // Sets the application to the engine
    engine.setApp(std::make_unique<Sandbox>("Sandbox Application"));

    // Runs the game loop
    engine.run();
}