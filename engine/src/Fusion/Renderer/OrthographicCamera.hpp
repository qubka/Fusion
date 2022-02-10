#pragma once

#include "Camera.hpp"

namespace Fusion {
    class FUSION_API OrthographicCamera : public Camera {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);
         ~OrthographicCamera() override = default;

        void setProjection(float left, float right, float bottom, float top);

        const glm::mat4& getProjection() const { return projectionMatrix; };
        const glm::mat4& getView() const { return viewMatrix; };
        const glm::mat4& getViewProjection() const { return viewProjectionMatrix; };

        const glm::vec3& getPosition() const { return position; };
        void setPosition(const glm::vec3& pos);
        float getRotation() const { return rotation; };
        void setRotation(float rot);
        void setPositionAndRotation(const glm::vec3& pos, float rot);

    private:
        glm::vec3 position{0};
        float rotation{0};

        void updateViewMatrix();
    };
}
