#pragma once

#include "perspective_camera.hpp"

namespace fe {
    class StereoCamera : public PerspectiveCamera {
    public:
        StereoCamera() = default;
        StereoCamera(const glm::vec2& size, float fov)
            : PerspectiveCamera{size, fov} {}
        StereoCamera(const glm::vec2& size, float fov, float near, float far)
            : PerspectiveCamera{size, fov, near, far} {}

        //! Returns the current convergence, which is the distance at which there is no parallax.
        float getConvergence() const { return convergence; }
        //! Sets the convergence of the camera, which is the distance at which there is no parallax.
        void setConvergence(float distance, bool adjustEyeSeparation = false);

        //! Returns the distance between the camera's for the left and right eyes.
        float getEyeSeparation() const { return eyeSeparation; }
        //! Sets the distance between the camera's for the left and right eyes. This affects the parallax effect.
        void setEyeSeparation( float distance );
        //! Returns the location of the currently enabled eye camera.
        glm::vec3 getEyePointShifted() const;

        //! Enables the left eye camera.
        void enableStereoLeft() { isStereo = true; isLeft = true; }
        //! Returns whether the left eye camera is enabled.
        bool isStereoLeftEnabled() const { return isStereo && isLeft; }
        //! Enables the right eye camera.
        void enableStereoRight() { isStereo = true; isLeft = false; }
        //! Returns whether the right eye camera is enabled.
        bool isStereoRightEnabled() const { return isStereo && ! isLeft; }
        //! Disables stereoscopic rendering, converting the camera to a standard CameraPersp.
        void disableStereo() { isStereo = false; }
        //! Returns whether stereoscopic rendering is enabled.
        bool isStereoEnabled() const { return isStereo; }

        void getNearClipCoordinates(glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight) const override { return getShiftedClipCoordinates(nearClip, 1.0f, topLeft, topRight, bottomLeft, bottomRight); }
        void getFarClipCoordinates(glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight) const override { return getShiftedClipCoordinates(farClip, farClip / nearClip, topLeft, topRight, bottomLeft, bottomRight); }

        const glm::mat4& getProjectionMatrix() const override;
        const glm::mat4& getInverseProjectionMatrix() const override;
        const glm::mat4& getViewMatrix() const override;
        const glm::mat4& getInverseViewMatrix() const override;

    protected:
        void calcViewMatrix() const override;
        void calcInverseView() const override;
        void calcProjection() const override;
        void calcInverseProjection() const override;

        void getShiftedClipCoordinates(float clipDist, float ratio, glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight) const;

        mutable glm::mat4 projectionMatrixLeft, inverseProjectionMatrixLeft;
        mutable glm::mat4 projectionMatrixRight, inverseProjectionMatrixRight;
        mutable glm::mat4 viewMatrixLeft, inverseModelViewMatrixLeft;
        mutable glm::mat4 viewMatrixRight, inverseModelViewMatrixRight;

    private:
        bool isStereo{ false };
        bool isLeft { true };
        float convergence{ 1.0f };
        float eyeSeparation{ 0.05f };
    };
}
