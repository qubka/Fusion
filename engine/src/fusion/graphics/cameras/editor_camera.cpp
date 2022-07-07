#include "editor_camera.hpp"
#include "fusion/devices/device_manager.hpp"

using namespace fe;

EditorCamera::EditorCamera() : PerspectiveCamera{} {
}

void EditorCamera::onUpdate() {
    auto window = DeviceManager::Get()->getWindow(0);

    if (!!window->getKey(Key::LeftAlt)) {
        if (!!window->getMouseButton(MouseButton::ButtonMiddle))
            mousePan(window->getMousePositionDelta());
        else if (!!window->getMouseButton(MouseButton::ButtonLeft))
            mouseRotate(window->getMousePositionDelta());
        else if (!!window->getMouseButton(MouseButton::ButtonRight))
            mouseZoom(window->getMousePositionDelta().y);
        else {
            float scroll = window->getMouseScrollDelta().y;
            if (scroll != 0.0f) {
                mouseZoom(scroll);
            }
        }

        setOrientation(glm::vec3{pitch, yaw, 0});
        setEyePoint(focalPoint - viewDirection * distance);
    }
}

void EditorCamera::mousePan(const glm::vec2& delta) {
    glm::vec2 pans{ panSpeed() };
    focalPoint += rightVector * delta.x * pans.x * distance;
    focalPoint += upVector * delta.y * pans.y * distance;
}

void EditorCamera::mouseRotate(const glm::vec2& delta) {
    float yawSign = upVector.y > 0 ? -1.0f : 1.0f;
    yaw -= yawSign * delta.x;
    pitch += delta.y;
}

void EditorCamera::mouseZoom(float delta) {
    distance -= delta * zoomSpeed();
    if (distance < 1.0f) {
        focalPoint += viewDirection;
        distance = 1.0f;
    }
}

glm::vec2 EditorCamera::panSpeed() const {
    float x = std::min(viewport.x / 1000.0f, 2.4f); // max = 2.4f
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = std::min(viewport.y / 1000.0f, 2.4f); // max = 2.4f
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return { xFactor, yFactor };
}

float EditorCamera::zoomSpeed() const {
    float dist = distance * 0.2f;
    dist = std::max(dist, 0.0f);
    float speed = dist * dist;
    speed = std::min(speed, 100.0f); // max speed = 100
    return speed;
}