#pragma once

#include "perspective_camera.hpp"

namespace fe {
    class KeyInput;
    class MouseInput;

    class EditorCamera : public PerspectiveCamera {
    public:
        EditorCamera();
        EditorCamera(float fov, float aspect, float near, float far);

        void update(float dt) override;

        float getPitch() const { return pitch; }
        float getYaw() const { return yaw; }
        float getDistance() const { return distance; }
        void setDistance(float value) { distance = value; }
        float getSpeed() const { return speed; }
        void setSpeed(float value) { speed = value; }

        void setViewport(const glm::vec2& size) {
            viewport = size;
            setAspect(size.x / size.y);
        }

        void mousePan(const glm::vec2& delta);
        void mouseRotate(const glm::vec2& delta);
        void mouseZoom(float delta);

        glm::vec3 calculatePosition() const;

        glm::vec2 panSpeed() const;
        float rotationSpeed() const;
        float zoomSpeed() const;

    private:
        glm::vec3 focalPoint{ 0.0f };
        glm::vec2 viewport{ 0.0f };

        float distance{ 10.0f };
        float speed{ 1.0f };
        float pitch{ 0.0f };
        float yaw{ 0.0f };
    };
}
