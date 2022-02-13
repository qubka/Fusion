#pragma once

#include "Fusion/Renderer/PerspectiveCamera.hpp"

namespace Fusion {
    class KeyInput;
    class MouseInput;

    class FUSION_API EditorCamera : public PerspectiveCamera {
    public:
        EditorCamera();
        EditorCamera(float fov, float aspect, float near, float far);
        ~EditorCamera() override = default;

        void onUpdate() override;

        float getPitch() const { return pitch; }
        float getYaw() const { return yaw; }
        float getDistance() const { return distance; }
        void setDistance(float value) { distance = value; }
        float getSpeed() const { return speed; }
        void setSpeed(float value) { speed = value; }

        //inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

    private:
        void mousePan(const glm::vec2& delta);
        void mouseRotate(const glm::vec2& delta);
        void mouseZoom(float delta);

        glm::vec3 calculatePosition() const;

        glm::vec2 panSpeed() const;
        float rotationSpeed() const;
        float zoomSpeed() const;
    private:
        glm::vec3 focalPoint = { 0.0f, 0.0f, 0.0f };

        float distance{10.0f};
        float speed{1.0f};
        float pitch{0.0f};
        float yaw{0.0f};
    };
}
