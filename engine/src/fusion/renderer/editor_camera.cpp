#include "editor_camera.hpp"
#include "fusion/input/input.hpp"

using namespace Fusion;

EditorCamera::EditorCamera() : PerspectiveCamera{45.0f, 1.778f, 0.1f, 1000.0f} {
}

EditorCamera::EditorCamera(float fov, float aspect, float near, float far) : PerspectiveCamera{fov,aspect,near,far} {
}

void EditorCamera::update(float ts) {
    if (Input::GetKey(Key::LeftAlt)) {
        if (Input::GetMouseButton(Mouse::ButtonMiddle))
            mousePan( Input::MouseDelta() * ts);
        else if (Input::GetMouseButton(Mouse::ButtonLeft))
            mouseRotate( Input::MouseDelta() * ts);
        else if (Input::GetMouseButton(Mouse::ButtonRight))
            mouseZoom(Input::MouseDelta().y * ts);
        else {
            auto scroll = Input::MouseScroll().y;
            if (scroll != 0.0f) {
                mouseZoom(scroll * 0.1f);
            }
        }

        setRotation(glm::quat{glm::vec3{pitch, yaw, 0}});
        setPosition(calculatePosition());
    }

    PerspectiveCamera::update(ts);
}

void EditorCamera::mousePan(const glm::vec2& delta) {
    auto speed = panSpeed();
    focalPoint += right * delta.x * speed.x * distance;
    focalPoint -= up * delta.y * speed.y * distance;
}

void EditorCamera::mouseRotate(const glm::vec2& delta) {
    float yawSign = up.y > 0 ? -1.0f : 1.0f;
    yaw += yawSign * delta.x;
    pitch -= delta.y;
}

void EditorCamera::mouseZoom(float delta) {
    distance -= delta * zoomSpeed();
    if (distance < 1.0f) {
        focalPoint += forward;
        distance = 1.0f;
    }
}

glm::vec2 EditorCamera::panSpeed() const {
    float x = std::min(viewportWidth / 1000.0f, 2.4f); // max = 2.4f
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = std::min(viewportHeight / 1000.0f, 2.4f); // max = 2.4f
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

glm::vec3 EditorCamera::calculatePosition() const {
    return (focalPoint - forward * distance);
}
