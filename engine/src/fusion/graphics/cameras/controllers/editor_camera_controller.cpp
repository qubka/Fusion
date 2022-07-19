#include "editor_camera_controller.hpp"

#include "fusion/core/time.hpp"
#include "fusion/input/input.hpp"
#include "fusion/devices/device_manager.hpp"
#include "fusion/graphics/cameras/camera.hpp"
#include "fusion/utils/glm_extention.hpp"

using namespace fe;

EditorCameraController::EditorCameraController() {
    mouseSensitivity = 0.00001f;
    zoomDampeningFactor = 0.00001f;
    dampeningFactor = 0.00001f;
    rotateDampeningFactor = 0.0000001f;
}

EditorCameraController::~EditorCameraController() {
}

void EditorCameraController::update(Camera& camera) {
    float dt = Time::DeltaTime().asSeconds();

    handleMouse(camera, dt);
    handleScroll(camera, dt);
    handleKeyboard(camera, dt);
}

void EditorCameraController::handleMouse(Camera& camera, float dt) {
    const glm::vec2& pos = Input::Get()->getMousePosition();
    auto window = DeviceManager::Get()->getWindow(0);

    if (camera.isOrthographic()) {
        if (Input::Get()->getMouseButton(MouseButton::ButtonRight)) {
            mouseSensitivity = 0.0005f;
            glm::vec3 position = camera.getEyePoint();
            position.x -= (pos.x - previousCurserPos.x) /** camera->getScale() */ * mouseSensitivity * 0.5f;
            position.y += (pos.y - previousCurserPos.y) /** camera->getScale() */ * mouseSensitivity * 0.5f;
            camera.setEyePoint(position);
        }
    } else {
        static bool mouseHeld = false;
        if (Input::Get()->getMouseButtonDown(MouseButton::ButtonRight)) {
            mouseHeld = true;
            window->setCursorHidden(true);
            storedCursorPos = pos;
            previousCurserPos = storedCursorPos;
        }

        if (Input::Get()->getMouseButton(MouseButton::ButtonRight)) {
            mouseSensitivity = 0.0002f;
            rotateVelocity = glm::vec2{(pos.x - previousCurserPos.x), (pos.y - previousCurserPos.y)} * mouseSensitivity * 10.0f;
        } else {
            if (mouseHeld) {
                mouseHeld = false;
                window->setCursorHidden(false);
                window->setMousePosition(storedCursorPos);
            }
        }

        if (glm::length(rotateVelocity) > FLT_EPSILON) {
            glm::quat orientation = camera.getOrientation();
            glm::quat rotationX = glm::angleAxis(rotateVelocity.y, vec3::right);
            glm::quat rotationY = glm::angleAxis(-rotateVelocity.x, vec3::up);

            orientation = rotationY * orientation;
            orientation = orientation * rotationX;

            previousCurserPos = pos;
            camera.setOrientation(orientation);
        }
    }

    rotateVelocity *= std::pow(rotateDampeningFactor, dt);
}

void EditorCameraController::handleKeyboard(Camera& camera, float dt) {
    if (camera.isOrthographic()) {
        cameraSpeed = /*camera->GetScale() **/ dt * 20.0f;

        if (Input::Get()->getKey(Key::A)) {
            velocity -= vec3::right * cameraSpeed;
        }

        if (Input::Get()->getKey(Key::D)) {
            velocity += vec3::right * cameraSpeed;
        }

        if (Input::Get()->getKey(Key::W)) {
            velocity += vec3::up * cameraSpeed;
        }

        if (Input::Get()->getKey(Key::S)) {
            velocity -= vec3::up * cameraSpeed;
        }

        if (glm::length(velocity) > FLT_EPSILON) {
            glm::vec3 position = camera.getEyePoint();
            position += velocity * dt;
            velocity *= std::pow(dampeningFactor, dt);

            camera.setEyePoint(position);
        }
    } else {

        float multiplier = 1000.0f;

        if (Input::Get()->getKey(Key::LeftShift)) {
            multiplier = 10000.0f;
        } else if (Input::Get()->getKey(Key::LeftAlt)) {
            multiplier = 50.0f;
        }

        cameraSpeed = multiplier * dt;

        if (Input::Get()->getMouseButton(MouseButton::ButtonRight)) {
            if (Input::Get()->getKey(Key::W)) {
                velocity -= camera.getForwardDirection() * cameraSpeed;
            }

            if (Input::Get()->getKey(Key::S)) {
                velocity += camera.getForwardDirection() * cameraSpeed;
            }

            if (Input::Get()->getKey(Key::A)) {
                velocity -= camera.getRightDirection() * cameraSpeed;
            }

            if (Input::Get()->getKey(Key::D)) {
                velocity += camera.getRightDirection() * cameraSpeed;
            }

            if (Input::Get()->getKey(Key::Q)) {
                velocity -= camera.getUpDirection() * cameraSpeed;
            }

            if (Input::Get()->getKey(Key::E)) {
                velocity += camera.getUpDirection() * cameraSpeed;
            }
        }

        if (glm::length(velocity) > FLT_EPSILON) {
            glm::vec3 position = camera.getEyePoint();
            position -= velocity * dt;
            camera.setEyePoint(position);
            velocity *= std::pow(dampeningFactor, dt);
        }
    }
}

void EditorCameraController::handleScroll(Camera& camera, float dt) {
    auto offset =  Input::Get()->getMouseScroll().y;

    if (camera.isOrthographic()) {
        float multiplier = Input::Get()->getKey(Key::LeftShift) ? 10.0f : 2.0f;

        if (offset != 0.0f) {
            zoomVelocity += dt * offset * multiplier;
        }

        if (!glm::epsilonEqual(zoomVelocity, 0.0f, FLT_EPSILON)) {
            float scale = 1.0f; //camera->getScale();

            scale -= zoomVelocity;

            if (scale < 0.15f) {
                scale = 0.15f;
                zoomVelocity = 0.0f;
            } else {
                zoomVelocity *= std::pow(zoomDampeningFactor, dt);
            }

            //camera->setScale(scale);
        }
    } else {
        if (offset != 0.0f) {
            zoomVelocity -= dt * offset * 10.0f;
        }

        if (!glm::epsilonEqual(zoomVelocity, 0.0f, FLT_EPSILON)) {
            glm::vec3 pos = camera.getEyePoint();
            pos -= camera.getForwardDirection() * zoomVelocity;
            zoomVelocity *= std::pow(zoomDampeningFactor, dt);
            camera.setEyePoint(pos);
        }
    }
}

void EditorCameraController::stopMovement() {
    zoomVelocity = 0.0f;
    velocity = vec3::zero;
    rotateVelocity = vec2::zero;
}
