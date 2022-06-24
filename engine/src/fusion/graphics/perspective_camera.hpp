#pragma once

#include "camera.hpp"
#include "fusion/utils/glm_extention.hpp"

namespace fe {
    class PerspectiveCamera : public Camera {
    public:
        PerspectiveCamera(float fov, float aspect, float near, float far);

        virtual void update(float dt) { updateView(); }

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
        void setPosition(const glm::vec3& pos) { position = pos; dirty = true; };
        const glm::quat& getRotation() const { return rotation; }
        void setRotation(const glm::quat& rot) { rotation = rot; updateDirs(); dirty = true; };

    private:
        void updateView();
        void updateProjection();
        void updateDirs();

    protected:
        glm::vec3 position{ 0.0f };
        glm::quat rotation{ quat::identity };

        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;

        float aspect;
        float fovy;
        float near;
        float far;
    };
}
