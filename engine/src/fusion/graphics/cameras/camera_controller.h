#pragma once

namespace fe {
    class Camera;
    class FUSION_API CameraController {
    public:
        CameraController() = default;
        virtual ~CameraController() = default;

        virtual void update(Camera& camera) = 0;

        void setMouseSensitivity(float value) { mouseSensitivity = value; }

        const glm::vec3& getVelocity() const { return velocity; }

    protected:
        glm::vec3 velocity{ 0.0f };
        glm::vec2 rotateVelocity{ 0.0f };
        //glm::vec3 focalPoint{ 0.0f };

        float zoomVelocity{ 0.0f };
        float cameraSpeed{ 0.0f };
        float distance{ 0.0f };
        //float zoom{ 1.0f };

        glm::vec2 projectionOffset{ 0.0f };
        glm::vec2 previousCurserPos{ 0.0f };
        float mouseSensitivity{ 0.1f };

        float zoomDampeningFactor{ 0.00001f };
        float dampeningFactor{ 0.00001f };
        float rotateDampeningFactor{ 0.001f };
    };
}
