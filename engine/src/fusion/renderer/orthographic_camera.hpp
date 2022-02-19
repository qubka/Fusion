#pragma once

#include "Camera.hpp"

namespace Fusion {
    class OrthographicCamera : public Camera {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);
         ~OrthographicCamera() override = default;

        virtual void onUpdate() { updateView(); }

        void setProjection(float left, float right, float bottom, float top);

        const glm::vec3& getPosition() const { return position; }
        void setPosition(const glm::vec3& pos);
        float getRotation() const { return rotation; }
        void setRotation(float rot);
        void setPositionAndRotation(const glm::vec3& pos, float rot);

    private:
        void updateView();

    private:
        glm::vec3 position{0};
        float rotation{0};
    };
}
