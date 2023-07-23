#include "editor_camera_controller.h"

#include "fusion/input/input.h"
#include "fusion/devices/device_manager.h"
#include "fusion/graphics/cameras/camera.h"

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
    auto input = Input::Get();
    const glm::vec2& pos = input->getMousePosition();
    auto window = DeviceManager::Get()->getWindow(0);

    static bool mouseHeld = false;
    if (input->getMouseButtonDown(MouseButton::ButtonRight)) {
        mouseHeld = true;
        window->setCursorHidden(true);
        //window->setCursor(manager->getCursor(static_cast<size_t>(CursorStandard::Crosshair)));
        storedCursorPos = pos;
        previousCurserPos = storedCursorPos;
    }

    if (camera.isOrthographic()) {
        if (input->getMouseButton(MouseButton::ButtonRight)) {
            mouseSensitivity = 0.0005f;
            glm::vec3 position{ camera.getEyePoint() };
            position.x -= (pos.x - previousCurserPos.x) * /*camera.getScale() **/ mouseSensitivity * 10.0f;
            position.z -= (pos.y - previousCurserPos.y) * /*camera.getScale() **/ mouseSensitivity * 10.0f;
            camera.setEyePoint(position);
            previousCurserPos = pos;
        } else {
            if (mouseHeld) {
                mouseHeld = false;
                window->setCursorHidden(false);
                //window->setCursor(nullptr);
                window->setMousePosition(storedCursorPos);
            }
        }
    } else {
        if (input->getMouseButton(MouseButton::ButtonRight)) {
            mouseSensitivity = 0.0002f;
            rotateVelocity = glm::vec2{(pos.x - previousCurserPos.x), (pos.y - previousCurserPos.y)} * mouseSensitivity * 10.0f;
        } else {
            if (mouseHeld) {
                mouseHeld = false;
                window->setCursorHidden(false);
                //window->setCursor(nullptr);
                window->setMousePosition(storedCursorPos);
            }
        }

        if (glm::length(rotateVelocity) > FLT_EPSILON) {
            glm::quat orientation{ camera.getOrientation() };
            glm::quat rotationX{ glm::angleAxis(rotateVelocity.y, vec3::right) };
            glm::quat rotationY{ glm::angleAxis(-rotateVelocity.x, vec3::up) };

            orientation = rotationY * orientation;
            orientation = orientation * rotationX;

            previousCurserPos = pos;
            camera.setOrientation(orientation);
        }
    }

    rotateVelocity *= glm::pow(rotateDampeningFactor, dt);
}

void EditorCameraController::handleKeyboard(Camera& camera, float dt) {
    auto input = Input::Get();
    
    if (camera.isOrthographic()) {
        cameraSpeed = camera.getScale() * dt * 20.0f;

        if (input->getKey(Key::A)) {
            velocity += camera.getRightDirection() * cameraSpeed;
        }

        if (input->getKey(Key::D)) {
            velocity -= camera.getRightDirection() * cameraSpeed;
        }

        if (input->getKey(Key::W)) {
            velocity += camera.getUpDirection() * cameraSpeed;
        }

        if (input->getKey(Key::S)) {
            velocity -= camera.getUpDirection() * cameraSpeed;
        }

        if (input->getKey(Key::Q)) {
            velocity -= camera.getForwardDirection() * cameraSpeed;
        }

        if (input->getKey(Key::E)) {
            velocity += camera.getForwardDirection() * cameraSpeed;
        }

        if (glm::length(velocity) > FLT_EPSILON) {
            glm::vec3 position{ camera.getEyePoint() };
            position += velocity * dt;
            velocity *= glm::pow(dampeningFactor, dt);

            camera.setEyePoint(position);
        }
    } else {
        float multiplier = 1000.0f;

        if (input->getKey(Key::LeftShift)) {
            multiplier = 10000.0f;
        } else if (input->getKey(Key::LeftAlt)) {
            multiplier = 50.0f;
        }

        cameraSpeed = multiplier * dt;

        if (input->getMouseButton(MouseButton::ButtonRight)) {
            if (input->getKey(Key::W)) {
                velocity -= camera.getForwardDirection() * cameraSpeed;
            }

            if (input->getKey(Key::S)) {
                velocity += camera.getForwardDirection() * cameraSpeed;
            }

            if (input->getKey(Key::A)) {
                velocity -= camera.getRightDirection() * cameraSpeed;
            }

            if (input->getKey(Key::D)) {
                velocity += camera.getRightDirection() * cameraSpeed;
            }

            if (input->getKey(Key::Q)) {
                velocity -= camera.getUpDirection() * cameraSpeed;
            }

            if (input->getKey(Key::E)) {
                velocity += camera.getUpDirection() * cameraSpeed;
            }
        }

        if (glm::length(velocity) > FLT_EPSILON) {
            glm::vec3 position{ camera.getEyePoint() };
            position -= velocity * dt;
            camera.setEyePoint(position);
            velocity *= glm::pow(dampeningFactor, dt);
        }
    }
}

void EditorCameraController::handleScroll(Camera& camera, float dt) {
    auto input = Input::Get();
    auto offset =  input->getMouseScroll().y;

    if (camera.isOrthographic()) {
        float multiplier = input->getKey(Key::LeftShift) ? 10.0f : 2.0f;

        if (offset != 0.0f) {
            zoomVelocity += dt * offset * multiplier;
        }

        if (!glm::epsilonEqual(zoomVelocity, 0.0f, FLT_EPSILON)) {
            float scale = camera.getScale();

            scale -= zoomVelocity;

            if (scale < 0.15f) {
                scale = 0.15f;
                zoomVelocity = 0.0f;
            } else {
                zoomVelocity *= glm::pow(zoomDampeningFactor, dt);
            }

            camera.setScale(scale);
        }
    } else {
        if (offset != 0.0f) {
            zoomVelocity -= dt * offset * 10.0f;
        }

        if (!glm::epsilonEqual(zoomVelocity, 0.0f, FLT_EPSILON)) {
            glm::vec3 position{ camera.getEyePoint() };
            position -= camera.getForwardDirection() * zoomVelocity;
            zoomVelocity *= glm::pow(zoomDampeningFactor, dt);
            camera.setEyePoint(position);
        }
    }
}

void EditorCameraController::stopMovement() {
    zoomVelocity = 0.0f;
    velocity = vec3::zero;
    rotateVelocity = vec2::zero;
}
