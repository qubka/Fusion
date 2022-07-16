#pragma once

#include "fusion/graphics/cameras/camera_controller.hpp"

namespace fe {
    class EditorCameraController  : public CameraController {
    public:
        explicit EditorCameraController();
        ~EditorCameraController() override;

        void update(Camera& camera) override;
        void stopMovement();

    private:
        void handleMouse(Camera& camera, float dt);
        void handleKeyboard(Camera& camera, float dt);
        void handleScroll(Camera& camera, float dt);

        glm::vec2 storedCursorPos{ FLT_MAX };
    };
}
