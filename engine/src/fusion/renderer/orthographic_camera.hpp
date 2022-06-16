#pragma once

#include "camera.hpp"

namespace fe {
    class OrthographicCamera : public Camera {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        virtual void update(float dt) { updateView(); }

        void setOrthographic(float left, float right, float bottom, float top);

        const glm::vec3& getPosition() const { return position; }
        void setPosition(const glm::vec3& pos) { position = pos; dirty = true; };
        float getRotation() const { return rotation; }
        void setRotation(float rot) { rotation = rot; dirty = true;};

    private:
        void updateView();

    private:
        glm::vec3 position{ 0.0f };
        float rotation{ 0.0f };
    };
}
