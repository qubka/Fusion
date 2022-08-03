#pragma once

#include "fusion/geometry/frustum.hpp"
#include "fusion/geometry/ray.hpp"

namespace fe {
    class Camera {
    public:
        Camera();
        Camera(float fov, float near, float far, float aspect);
        Camera(const glm::vec3& position, float fov, float near, float far, float aspect);
        Camera(float aspect, float scale);
        Camera(float aspect, float near, float far);
        ~Camera() = default;

        //! Returns the position in world-space from which the Camera is viewing
        glm::vec3 getEyePoint() const { return eyePoint; }
        //! Sets the position in world-space from which the Camera is viewing
        bool setEyePoint(const glm::vec3& eyePoint);

        //! Returns the vector in world-space which represents "up" - typically glm::vec3( 0, 1, 0 )
        glm::vec3 getWorldUp() const { return worldUp; }
        //! Sets the vector in world-space which represents "up" - typically glm::vec3( 0, 1, 0 )
        bool setWorldUp(glm::vec3 worldUp);

        //! Modifies the view direction to look from the current eyePoint to \a target. Also updates the pivot distance.
        bool lookAt(glm::vec3 target);
        //! Modifies the eyePoint and view direction to look from \a eyePoint to \a target. Also updates the pivot distance.
        bool lookAt(const glm::vec3& eyePoint, glm::vec3 target);
        //! Modifies the eyePoint and view direction to look from \a eyePoint to \a target with up vector \a up (to achieve camera roll). Also updates the pivot distance.
        bool lookAt(const glm::vec3& eyePoint, glm::vec3 target, glm::vec3 up);

        //! Returns the world-space vector along which the camera is oriented
        const glm::vec3& getViewDirection() const { return viewDirection; }
        //! Sets the world-space vector along which the camera is oriented
        bool setViewDirection(glm::vec3 viewDirection);

        //! Returns the world-space glm::quaternion that expresses the camera's orientation
        const glm::quat& getOrientation() const { return orientation; }
        //! Sets the camera's orientation with world-space glm::quaternion \a orientation
        bool setOrientation(glm::quat orientation);

        //! Returns whether the camera represents an orthographic projection instead of an perspective
        bool isOrthographic() const { return orthographic; }
        //! Switch the camera mode to orthographic projection
        bool setOrthographic(bool flag);

        //! Returns the camera's focal length, calculating it based on the field of view.
        float getFocalLength() const { return 1.0f / (glm::tan(glm::radians(fovDegrees) * 0.5f) * 2.0f); } /// @link http://paulbourke.net/miscellaneous/lens/

        //! Primarily for user interaction, such as with CameraUi. Returns the distance from the camera along the view direction relative to which tumbling and dollying occur.
        float getPivotDistance() const { return pivotDistance; }
        //! Primarily for user interaction, such as with CameraUi. Sets the distance from the camera along the view direction relative to which tumbling and dollying occur.
        void setPivotDistance(float distance) { pivotDistance = distance; }

        //! Primarily for user interaction, such as with CameraUi. Returns the world-space point relative to which tumbling and dollying occur.
        glm::vec3 getPivotPoint() const { return eyePoint - viewDirection * pivotDistance; }

        //! Returns the Camera's frustum object using current view and current projection matrices
        const Frustum& getFrustum() { if (frustumDirty) calcFrustum(); return frustum; }
        //! Returns the Camera's Projection matrix, which converts view-space into clip-space
        const glm::mat4& getProjectionMatrix() const { if (projectionDirty) calcProjection(); return projectionMatrix; }
        //! Returns the Camera's Inverse Projection matrix, which converts view-space into clip-space
        const glm::mat4& getInverseProjectionMatrix() const { if (inverseProjectionDirty) calcInverseProjection(); return inverseProjectionMatrix; }
        //! Returns the Camera's View matrix, which converts world-space into view-space
        const glm::mat4& getViewMatrix() const { if (viewDirty) calcView(); return viewMatrix; }
        //! Returns the Camera's Inverse View matrix, which converts view-space into world-space
        const glm::mat4& getInverseViewMatrix() const { if (inverseViewDirty) calcInverseView(); return inverseViewMatrix; }

        //! Returns the camera's vertical field of view measured in degrees.
        float getFov() const { return fovDegrees; }
        //! Sets the camera's vertical field of view measured in degrees.
        bool setFov(float value);

        //! Returns the camera's horizontal field of view measured in degrees.
        float getFovHorizontal() const { return glm::degrees(2.0f * std::atan(std::tan(glm::radians(fovDegrees) * 0.5f) * aspectRatio)); }
        //! Sets the camera's horizontal field of view measured in degrees.
        bool setFovHorizontal(float value);

        //! Returns the aspect ratio of the image plane - its width divided by its height
        float getAspectRatio() const { return aspectRatio; }
        //! Sets the aspect ratio of the image plane - its width divided by its height
        bool setAspectRatio(float value);

        //! Returns the distance along the view direction to the Near clipping plane.
        float getNearClip() const { return nearClip; }
        //! Sets the distance along the view direction to the Near clipping plane.
        bool setNearClip(float value);

        //! Returns the distance along the view direction to the Far clipping plane.
        float getFarClip() const { return farClip; }
        //! Sets the distance along the view direction to the Far clipping plane.
        bool setFarClip(float value);

        //! Returns the scale distance of the Orthographic projection
        float getScale() const { return scale; }
        //! Sets the scale distance to the Orthographic projection
        bool setScale(float value);

        //! Converts a world-space coordinate \a worldCoord to screen coordinates as viewed by the camera, based on a screen which is \a screen.width x \a screen.height pixels.
        glm::vec2 worldToScreen(const glm::vec3& worldCoord, const glm::vec2& screenSize) const;
        //! Converts a eye-space coordinate \a eyeCoord to screen coordinates as viewed by the camera
        glm::vec2 eyeToScreen(const glm::vec3& eyeCoord, const glm::vec2& screenSize) const;
        //! Converts a screen coordinates as viewed by the camera, based on a screen which is \a screen.width x \a screen.height pixels to a world-space coordinate \a worldCoord.
        glm::vec3 screenToWorld(const glm::vec2& screenCoord, const glm::vec2& screenSize, bool flipY = false) const;
        //! Converts a screen coordinates as viewed by the camera, based on a screen which is \a screen.width x \a screen.height pixels to a world-space ray.
        Ray screenPointToRay(const glm::vec2& screenCoord, const glm::vec2& screenSize, bool flipY = false) const; // alternative to generateRay

        //! Converts a world-space coordinate \a worldCoord to eye-space, also known as camera-space. -Z is along the view direction.
        glm::vec3 worldToEye(const glm::vec3& worldCoord) const { return { getViewMatrix() * glm::vec4{worldCoord, 1} }; }
        //! Converts a world-space coordinate \a worldCoord to the z axis of eye-space, also known as camera-space. -Z is along the view direction. Suitable for depth sorting.
        float worldToEyeDepth(const glm::vec3& worldCoord) const;
        //! Converts a world-space coordinate \a worldCoord to normalized device coordinates
        glm::vec3 worldToNdc(const glm::vec3& worldCoord) const;

        //! Gets the camera rotation dirs
        const glm::vec3& getForwardDirection() const { return viewDirection; }
        const glm::vec3& getRightDirection() const { return rightVector; }
        const glm::vec3& getUpDirection() const { return upVector; }

        ///@link https://google.github.io/filament/Filament.html
        float getAperture() const { return aperture; }
        void setAperture(float value) { aperture = value; }
        float getShutterSpeed() const { return shutterSpeed; }
        void setShutterSpeed(float value) { shutterSpeed = value; }
        float getSensitivity() const { return sensitivity; }
        void setSensitivity(float value) { sensitivity = value; }
        float getEv100() const { return std::log2((aperture * aperture) / shutterSpeed * 100.0f / sensitivity); }
        float getExposure() const { return 1.0f / (std::pow(2.0f, getEv100()) * 1.2f); }
        float getShadowBoundingRadius() const { return shadowBoundingRadius; }

    protected:
        void calcProjection() const;
        void calcInverseProjection() const;
        void calcView() const;
        void calcInverseView() const;
        void calcFrustum();

        void dirtyView() const { viewDirty = inverseViewDirty = frustumDirty = true; }
        bool isViewDirty() const { return viewDirty || inverseViewDirty; }
        void dirtyProjection() const { projectionDirty = inverseProjectionDirty = frustumDirty = true; }
        bool isProjectionDirty() const { return projectionDirty || inverseProjectionDirty; }

    protected:
        glm::vec3 eyePoint{ vec3::zero };
        glm::vec3 viewDirection{ vec3::forward };
        glm::quat orientation{ quat::identity };
        glm::vec3 worldUp{ vec3::up };

        //mutable glm::vec3 forwardVector;
        mutable glm::vec3 rightVector;
        mutable glm::vec3 upVector;

        mutable glm::mat4 projectionMatrix;
        mutable glm::mat4 inverseProjectionMatrix;
        mutable glm::mat4 viewMatrix;
        mutable glm::mat4 inverseViewMatrix;

        float fovDegrees{ 60.0f };
        float nearClip{ 0.001f };
        float farClip{ 1000.0f };
        float aspectRatio{ 10.0f };
        float pivotDistance{ 1.0f };
        float scale{ 10.0f };
        float aperture{ 50.0f };
        float shutterSpeed{ 1.0f / 60.0f };
        float sensitivity{ 200.0f };
        float shadowBoundingRadius{ 10.0f };

        bool orthographic{ false };

        mutable bool projectionDirty{ true };
        mutable bool inverseProjectionDirty{ true };
        mutable bool viewDirty{ true };
        mutable bool inverseViewDirty{ true };
        mutable bool frustumDirty{ true };

        Frustum frustum;
    };
}