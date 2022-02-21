#pragma once

#include "camera.hpp"

namespace Fusion {
    class OrthographicCamera : public Camera {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);
         ~OrthographicCamera() override = default;

        virtual void update(float ts) { updateView(); }

        void setOrthographic(float left, float right, float bottom, float top);

        const glm::vec3& getPosition() const { return position; }
        void setPosition(const glm::vec3& pos) { position = pos; isDirty = true; };
        float getRotation() const { return rotation; }
        void setRotation(float rot) { rotation = rot; isDirty = true;};

    private:
        void updateView();

    private:
        glm::vec3 position{ 0 };
        float rotation{ 0 };
    };
}
