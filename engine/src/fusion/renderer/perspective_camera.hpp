#pragma once

#include "camera.hpp"

namespace fe {
    class PerspectiveCamera : public Camera {
    public:
        PerspectiveCamera(float fov, float aspect, float near, float far);

        virtual void update(float dt) { updateView(); }

        void setViewport(const glm::vec2& size);
        void setPerspective(float fov, float aspect, float near, float far);

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
        void setPosition(const glm::vec3& pos) { position = pos; isDirty = true; };
        const glm::quat& getRotation() const { return rotation; }
        void setRotation(const glm::quat& rot) { rotation = rot; updateDirs(); isDirty = true; };

    private:
        void updateView();
        void updateProjection();
        void updateDirs();

    protected:
        glm::vec3 position{ 0.0f};
        glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };

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
