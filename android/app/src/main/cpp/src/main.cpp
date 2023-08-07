#include <game-activity/native_app_glue/android_native_app_glue.h>

#include "platform/android/android_engine.h"

#include "sandbox.h"

void android_main(struct android_app* state) {
    using namespace fe;

    // Creates the engine
    android::Engine engine{state, {}};

    // Sets the application to the engine
    engine.setApp(std::make_unique<Sandbox>("Sandbox Application"));

    // Runs the game loop
    engine.run();
}