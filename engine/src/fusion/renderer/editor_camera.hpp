#pragma once

#include "perspective_camera.hpp"

namespace Fusion {
    class KeyInput;
    class MouseInput;

    class EditorCamera : public PerspectiveCamera {
    public:
        EditorCamera();
        EditorCamera(float fov, float aspect, float near, float far);
        ~EditorCamera() override = default;

        void update() override;

        float getPitch() const { return pitch; }
        float getYaw() const { return yaw; }
        float getDistance() const { return distance; }
        void setDistance(float value) { distance = value; }
        float getSpeed() const { return speed; }
        void setSpeed(float value) { speed = value; }

        void mousePan(const glm::vec2& delta);
        void mouseRotate(const glm::vec2& delta);
        void mouseZoom(float delta);

        glm::vec3 calculatePosition() const;

        glm::vec2 panSpeed() const;
        float rotationSpeed() const;
        float zoomSpeed() const;

    private:
        glm::vec3 focalPoint{ 0 };

        float distance{ 10 };
        float speed{ 1 };
        float pitch{ 0 };
        float yaw{ 0 };
    };
}
