#include "window.hpp"
// TODO: Refactor
#if defined(ANDROID)
#include "fusion/events/application_events.hpp"
#include "fusion/events/window_events.hpp"
#include "fusion/events/key_events.hpp"
#include "fusion/events/mouse_events.hpp"
#include "fusion/input/input.hpp"
#include "fusion/input/key_input.hpp"
#include "fusion/input/mouse_input.hpp"

#include <vulkan/vulkan.hpp>

using namespace android;

Window* Window::instance{nullptr};

Window::Window()
    : fe::Window{ANativeWindow_getWidth(androidApp->window),
                 ANativeWindow_getHeight(androidApp->window)}
{
    assert(!instance && "Application already exists!");
    instance = this;
}

bool Window::shouldClose() {
    bool destroy = false;
    eventQueue.clean();
    resetInputs();
    focused = true;
    int ident, events;
    struct android_poll_source* source;
    while (!destroy && (ident = ALooper_pollAll(focused ? 0 : -1, NULL, &events, (void**)&source)) >= 0) {
        if (source != NULL) {
            source->process(androidApp, source);
        }
        destroy = androidApp->destroyRequested != 0;
    }

    // App destruction requested
    return destroy;
}

void Window::resetInputs() {
    /*
    fe::Input::Update();
    fe::MouseInput::Update();
    fe::KeyInput::Update();*/
}

int32_t Window::onInput(AInputEvent* event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        bool handled = false;
        glm::ivec2 touchPoint;
        int32_t eventSource = AInputEvent_getSource(event);
        switch (eventSource) {
            case AINPUT_SOURCE_TOUCHSCREEN: {
                int32_t action = AMotionEvent_getAction(event);

                switch (action) {
                    case AMOTION_EVENT_ACTION_UP:
                        mouseButtons.left = false;
                        break;

                    case AMOTION_EVENT_ACTION_DOWN:
                        // Detect double tap
                        mouseButtons.left = true;
                        mousePos.x = AMotionEvent_getX(event, 0);
                        mousePos.y = AMotionEvent_getY(event, 0);
                        break;

                    case AMOTION_EVENT_ACTION_MOVE:
                        touchPoint.x = AMotionEvent_getX(event, 0);
                        touchPoint.y = AMotionEvent_getY(event, 0);
                        mouseMoved(vec2{ touchPoint });
                        break;

                    default:
                        break;
                }
            }
                return 1;
        }
    }

    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        int32_t keyCode = AKeyEvent_getKeyCode((const AInputEvent*)event);
        int32_t action = AKeyEvent_getAction((const AInputEvent*)event);
        int32_t button = 0;

        if (action == AKEY_EVENT_ACTION_UP)
            return 0;

        switch (keyCode) {
            case AKEYCODE_BUTTON_A:
                keyPressed(GAMEPAD_BUTTON_A);
                break;
            case AKEYCODE_BUTTON_B:
                keyPressed(GAMEPAD_BUTTON_B);
                break;
            case AKEYCODE_BUTTON_X:
                keyPressed(GAMEPAD_BUTTON_X);
                break;
            case AKEYCODE_BUTTON_Y:
                keyPressed(GAMEPAD_BUTTON_Y);
                break;
            case AKEYCODE_BUTTON_L1:
                keyPressed(GAMEPAD_BUTTON_L1);
                break;
            case AKEYCODE_BUTTON_R1:
                keyPressed(GAMEPAD_BUTTON_R1);
                break;
            case AKEYCODE_BUTTON_START:
                paused = !paused;
                break;
        };
    }
    return 0;
}
#endif