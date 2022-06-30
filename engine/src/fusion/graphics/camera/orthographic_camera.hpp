#pragma once

#include "camera.hpp"

namespace fe {
    class OrthographicCamera : public Camera {
    public:
        //! Creates a default camera with eyePoint at ( 28, 21, 28 ), looking at the origin, 35deg vertical field-of-view and a 1.333 aspect ratio.
        OrthographicCamera();
        OrthographicCamera(float left, float right, float bottom, float top, float near, float far);
        ~OrthographicCamera() = default;

        void setOrthographic(float left, float right, float bottom, float top, float near, float far);

        bool isPerspective() const override { return false; }

        void getNearClipCoordinates(glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight) const override { getClipCoordinates(nearClip, 1.0f, topLeft, topRight, bottomLeft, bottomRight ); }
        void getFarClipCoordinates(glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight) const override { getClipCoordinates(farClip, 1.0f, topLeft, topRight, bottomLeft, bottomRight ); }

    protected:
        void calcProjection() const override;
        Ray	calcRay(const glm::vec2& uv, float imagePlaneAspectRatio) const override;
    };
}
