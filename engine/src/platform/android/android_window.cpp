#include "android_window.h"

#include "fusion/devices/device_manager.h"
#include "fusion/filesystem/storage.h"

#include <android/native_window.h>

using namespace android;

Window::Window(ANativeWindow* window, const fe::WindowInfo& windowInfo) : fe::Window{}
    , window{window}
    , size{ANativeWindow_getWidth(window), ANativeWindow_getHeight(window)}
    , title{windowInfo.title} {

    onStart.publish();
}

Window::~Window() {

}

void Window::onUpdate() {
    float dt = fe::Time::DeltaTime().asSeconds();

    // Updates the position delta.
    mousePositionDelta = dt * (mouseLastPosition - mousePosition);
    mouseLastPosition = mousePosition;

    // Updates the scroll delta.
    mouseScrollDelta = dt * (mouseLastScroll - mouseScroll);
    mouseLastScroll = mouseScroll;
}

VkResult Window::createSurface(VkInstance instance, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface) const {
    VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = { VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR };
    surfaceCreateInfo.window = window;
    return vkCreateAndroidSurfaceKHR(instance, &surfaceCreateInfo, nullptr, surface);
}

/*int32_t Window::onInput(AInputEvent* event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        bool handled = false;

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
                        cursorPosCallback(event);
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
}*/

/*namespace android {
    void Window::cursorPosCallback(AInputEvent* event) {
        glm::vec2 pos {AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0)};

        LOG_VERBOSE << "MouseMotionEvent: " << glm::to_string(pos);

        window.mousePosition = pos;
        window.onMouseMotion.publish(pos);

        const auto& size = window.getSize();
        if (size.x <= 1 || size.y <= 1) {
            return;
        }

        glm::vec2 norm{ 2.0f * (pos / glm::vec2{size}) - 1.0f};

        LOG_VERBOSE << "MouseMotionNormEvent: " << glm::to_string(norm);

        window.mousePositionNorm = norm;
        window.onMouseMotionNorm.publish(norm);
    }
}*/