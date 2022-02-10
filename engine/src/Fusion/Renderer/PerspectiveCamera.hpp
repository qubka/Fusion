#pragma once

#include "Camera.hpp"

namespace Fusion {
    class Ray;
    class Window;
    class WindowResizeEvent;
    class MouseScrollEvent;

    class FUSION_API PerspectiveCamera : public Camera {
    public:
        PerspectiveCamera(Window& window, float fov, float near, float far);
        ~PerspectiveCamera() override;

        virtual void onUpdate() {};

        // Camera oriented directions
        const glm::vec3& getForward() const { return forward; }
        const glm::vec3& getRight() const { return right; }
        const glm::vec3& getUp() const { return up; }

        float getFov() const { return fovy; };
        void setFov(float value);
        float getNearClip() const { return near; };
        void setNearClip(float zNear);
        float getFarClip() const { return far; };
        void setFarClip(float zFar);

        const glm::vec3& getPosition() const { return position; };
        void setPosition(const glm::vec3& pos);
        const glm::quat& getRotation() const { return rotation; };
        void setRotation(const glm::quat& rot);
        void setPositionAndRotation(const glm::vec3& pos, const glm::quat& rot);

        Ray screenPointToRay(const glm::vec2& pos) const;
        glm::vec3 screenToWorldPoint(const glm::vec2& pos) const;

    private:
        glm::vec3 calcForward() const;
        glm::vec3 calcRight() const;
        glm::vec3 calcUp() const;

        void onWindowResize(const WindowResizeEvent& event);

    protected:
        Window& window;

        glm::vec3 position{0};
        glm::quat rotation{1, 0, 0, 0};

        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;

        float fovy;
        float near;
        float far;

        void updateViewMatrix();
    };
}
