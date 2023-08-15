#include "android_engine.h"
#include "android.h"

using namespace fe::android;

extern "C" bool VulkanKeyEventFilter(const GameActivityKeyEvent* event) {
    return false;
}
extern "C" bool VulkanMotionEventFilter(const GameActivityMotionEvent* event) {
    return false;
}

Engine::Engine(struct android_app* state, CommandLineArgs&& args) : fe::Engine{std::move(args)}, app{state} {
    app->userData = this;
    app->onAppCmd = OnAppCmd;

    android_app_set_key_event_filter(app, VulkanKeyEventFilter);
    android_app_set_motion_event_filter(app, VulkanMotionEventFilter);
}

/**
 * Called by the Android runtime whenever events happen so the
 * app can react to it.
 */
void Engine::OnAppCmd(struct android_app* app, int32_t cmd) {
    auto engine = static_cast<Engine*>(app->userData);
    switch (cmd) {
        case APP_CMD_START:
            engine->init();
			engine->startup();
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (app->window != nullptr) {
				DeviceManager::Get()->getWindow(0).setNativeWindow(engine->app->window)
                engine->render = true;
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine->render = false;
			DeviceManager::Get()->getWindow(0).setNativeWindow(nullptr);
            break;
        case APP_CMD_DESTROY:
            // The window is being hidden or closed, clean it up.
            engine->running = false;
        default:
            break;
    }
}

/*
 * Process user touch and key events. GameActivity double buffers those events,
 * applications can process at any time. All of the buffered events have been
 * reported "handled" to OS. For details, refer to:
 * d.android.com/games/agdk/game-activity/get-started#handle-events
 */
static void HandleInputEvents(struct android_app* app) {
    auto inputBuf = android_app_swap_input_buffers(app);
    if (inputBuf == nullptr) {
        return;
    }

    // For the minimum, apps need to process the exit event (for example,
    // listening to AKEYCODE_BACK). This sample has done that in the Kotlin side
    // and not processing other input events, we just reset the event counter
    // inside the android_input_buffer to keep app glue code in a working state.
    android_app_clear_motion_events(inputBuf);
    android_app_clear_motion_events(inputBuf);
}

int32_t Engine::run() {
	int ident, events;
	android_poll_source* source;

	while (!started) {
        while ((ident = ALooper_pollAll(-1, nullptr, &events, (void **)&source)) >= 0) {
            if (source != nullptr) {
                source->process(app, source);
            }
        }

        if (app->destroyRequested != 0)
            return;
	}
	
    running = true;
    while (running) {
        while ((ident = ALooper_pollAll(render ? 0 : -1, nullptr, &events, (void **)&source)) >= 0) {
            if (source != nullptr) {
                source->process(app, source);
            }
        }

        if (app->destroyRequested != 0)
            break;

		updateMain();
    }
    shutdown();
    return 0;
}
