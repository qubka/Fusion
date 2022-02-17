#pragma once

#include "Camera.hpp"

namespace Fusion {
    class FUSION_API PerspectiveCamera : public Camera {
    public:
        PerspectiveCamera(float aspect, float fov, float near, float far);
        ~PerspectiveCamera() override = default;

        virtual void onUpdate() { updateView(); }

        void setViewport(int width, int height);
        void setProjection(float aspect, float fov, float near, float far);

        // Camera oriented directions
        const glm::vec3& getForward() const { return forward; }
        const glm::vec3& getRight() const { return right; }
        const glm::vec3& getUp() const { return up; }

        float getAspect() const { return aspect; }
        void setAspect(float value) { aspect = value; updateProjection(); }
        float getFov() const { return fovy; }
        void setFov(float value) { fovy = value; updateProjection(); }
        float getNearClip() const { return near; }
        void setNearClip(float zNear) { near = zNear; updateProjection(); }
        float getFarClip() const { return far; }
        void setFarClip(float zFar) { far = zFar; updateProjection(); }

        const glm::vec3& getPosition() const { return position; }
        void setPosition(const glm::vec3& pos) { position = pos; isDirty = true; }
        const glm::quat& getRotation() const { return rotation; }
        void setRotation(const glm::quat& rot) { rotation = rot; isDirty = true; }
        void setPositionAndRotation(const glm::vec3& pos, const glm::quat& rot) {
            position = pos;
            rotation = rot;
            isDirty = true;
        }

    private:
        glm::vec3 calcForward() const;
        glm::vec3 calcRight() const;
        glm::vec3 calcUp() const;

        void updateView();
        void updateProjection();

    protected:
        glm::vec3 position{0};
        glm::quat rotation{1, 0, 0, 0};

        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;

        float aspect;
        float fovy;
        float near;
        float far;

        float viewportWidth;
        float viewportHeight;
    };
}
