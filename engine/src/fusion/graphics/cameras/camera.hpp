#pragma once

#include "fusion/geometry/ray.hpp"
#include "fusion/geometry/sphere.hpp"
#include "fusion/geometry/frustum.hpp"
#include "fusion/utils/glm_extention.hpp"

namespace fe {
    //! Base Camera class, which manages the projection and view matrices for a 3-dimensional scene, as well as providing mapping functionality.
    class Camera {
    public:
        virtual ~Camera() = default;

        virtual void onUpdate() = 0;

        //! Returns the position in world-space from which the Camera is viewing
        glm::vec3 getEyePoint() const { return eyePoint; }
        //! Sets the position in world-space from which the Camera is viewing
        void setEyePoint(const glm::vec3& eyePoint);
        
        //! Returns the vector in world-space which represents "up" - typically glm::vec3( 0, 1, 0 ) 
        glm::vec3 getWorldUp() const { return worldUp; }
        //! Sets the vector in world-space which represents "up" - typically glm::vec3( 0, 1, 0 )
        void setWorldUp(glm::vec3 worldUp);

        //! Modifies the view direction to look from the current eyePoint to \a target. Also updates the pivot distance.
        void lookAt(glm::vec3 target);
        //! Modifies the eyePoint and view direction to look from \a eyePoint to \a target. Also updates the pivot distance.
        void lookAt(const glm::vec3& eyePoint, glm::vec3 target);
        //! Modifies the eyePoint and view direction to look from \a eyePoint to \a target with up vector \a up (to achieve camera roll). Also updates the pivot distance.
        void lookAt(const glm::vec3& eyePoint, glm::vec3 target, glm::vec3 up);

        //! Returns the world-space vector along which the camera is oriented
        const glm::vec3& getViewDirection() const { return viewDirection; }
        //! Sets the world-space vector along which the camera is oriented
        void setViewDirection(glm::vec3 viewDirection);

        //! Returns the world-space glm::quaternion that expresses the camera's orientation
        const glm::quat& getOrientation() const { return orientation; }
        //! Sets the camera's orientation with world-space glm::quaternion \a orientation
        void setOrientation(glm::quat orientation);

        //! Returns the camera's vertical field of view measured in degrees.
        float getFov() const { return fov; }
        //! Sets the camera's vertical field of view measured in degrees.
        void setFov(float verticalFov);

        //! Returns the camera's horizontal field of view measured in degrees.
        float getFovHorizontal() const { return glm::degrees(2.0f * std::atan(std::tan(glm::radians(fov) * 0.5f) * aspectRatio)); }
        //! Sets the camera's horizontal field of view measured in degrees.
        void setFovHorizontal(float horizontalFov);

        //! Returns the camera's focal length, calculating it based on the field of view.
        float getFocalLength() const;
        
        //! Primarily for user interaction, such as with CameraUi. Returns the distance from the camera along the view direction relative to which tumbling and dollying occur.
        float getPivotDistance() const { return pivotDistance; }
        //! Primarily for user interaction, such as with CameraUi. Sets the distance from the camera along the view direction relative to which tumbling and dollying occur.
        void setPivotDistance(float distance) { pivotDistance = distance; }

        //! Primarily for user interaction, such as with CameraUi. Returns the world-space point relative to which tumbling and dollying occur.
        glm::vec3 getPivotPoint() const { return eyePoint - viewDirection * pivotDistance; }

        //! Returns the aspect ratio of the image plane - its width divided by its height
        float getAspectRatio() const { return aspectRatio; }
        //! Sets the aspect ratio of the image plane - its width divided by its height
        void setAspectRatio(float aspect);

        //! Returns the distance along the view direction to the Near clipping plane.
        float getNearClip() const { return nearClip; }
        //! Sets the distance along the view direction to the Near clipping plane.
        void setNearClip(float near);

        //! Returns the distance along the view direction to the Far clipping plane.
        float getFarClip() const { return farClip; }
        //! Sets the distance along the view direction to the Far clipping plane.
        void setFarClip(float far);

        //! Returns the four corners of the Camera's Near clipping plane, expressed in world-space
        virtual void getNearClipCoordinates(glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight) const { return getClipCoordinates(nearClip, 1.0f, topLeft, topRight, bottomLeft, bottomRight); }
        //! Returns the four corners of the Camera's Far clipping plane, expressed in world-space
        virtual void getFarClipCoordinates(glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight) const { getClipCoordinates(farClip, farClip / nearClip, topLeft, topRight, bottomLeft, bottomRight); }

        //! Returns whether the camera represents a perspective projection instead of an orthographic
        virtual bool isPerspective() const = 0;
        //! Returns the coordinates of the camera's frustum, suitable for passing to \c glFrustum
        void getFrustum(float& left, float& top, float& right, float& bottom, float& near, float& far) const;
        //! Returns the Camera's Projection matrix, which converts view-space into clip-space
        virtual const glm::mat4& getProjectionMatrix() const { if (!projectionCached) calcProjection(); return projectionMatrix; }
        //! Returns the Camera's Inverse Projection matrix, which converts view-space into clip-space
        virtual const glm::mat4& getInverseProjectionMatrix() const { if (!inverseProjectionCached) calcInverseProjection(); return inverseProjectionMatrix; }
        //! Returns the Camera's View matrix, which converts world-space into view-space
        virtual const glm::mat4& getViewMatrix() const { if (!modelViewCached) calcViewMatrix(); return viewMatrix; }
        //! Returns th Camera's Inverse View matrix, which converts view-space into world-space
        virtual const glm::mat4& getInverseViewMatrix() const { if (!inverseModelViewCached) calcInverseView(); return inverseModelViewMatrix; }

        //! Returns a Ray that passes through the image plane coordinates (\a u, \a v) (expressed in the range [0,1]) on an image plane of aspect ratio \a imagePlaneAspectRatio
        Ray generateRay(const glm::vec2& uv, float imagePlaneAspectRatio) const { return calcRay(uv, imagePlaneAspectRatio); }
        //! Returns a Ray that passes through the pixels coordinates \a posPixels on an image of size \a imageSize
        Ray generateRay(const glm::vec2& pos, const glm::vec2& imageSize) const { return calcRay({pos.x / imageSize.x, (imageSize.y - pos.y) / imageSize.y}, imageSize.x / imageSize.y); }

        //! Returns the \a right and \a up vectors suitable for billboarding relative to the Camera
        void getBillboardVectors(glm::vec3& right, glm::vec3& up) const;

        //! Converts a world-space coordinate \a worldCoord to screen coordinates as viewed by the camera, based on a screen which is \a screen.width x \a screen.height pixels.
        glm::vec2 worldToScreen(const glm::vec3& worldCoord, const glm::vec2& screenSize) const;
        //! Converts a eye-space coordinate \a eyeCoord to screen coordinates as viewed by the camera
        glm::vec2 eyeToScreen(const glm::vec3& eyeCoord, const glm::vec2& screenSize) const;
        //! Converts a screen coordinates as viewed by the camera, based on a screen which is \a screen.width x \a screen.height pixels to a world-space coordinate \a worldCoord.
        glm::vec3 screenToWorld(const glm::vec2& screenCoord, const glm::vec2& screenSize) const;
        //! Converts a screen coordinates as viewed by the camera, based on a screen which is \a screen.width x \a screen.height pixels to a world-space ray.
        Ray screenPointToRay(const glm::vec2& screenCoord, const glm::vec2& screenSize) const;

        //! Converts a world-space coordinate \a worldCoord to eye-space, also known as camera-space. -Z is along the view direction.
        glm::vec3 worldToEye(const glm::vec3& worldCoord) const { return { getViewMatrix() * glm::vec4{worldCoord, 1} }; }
        //! Converts a world-space coordinate \a worldCoord to the z axis of eye-space, also known as camera-space. -Z is along the view direction. Suitable for depth sorting.
        float worldToEyeDepth(const glm::vec3& worldCoord) const;
        //! Converts a world-space coordinate \a worldCoord to normalized device coordinates
        glm::vec3 worldToNdc(const glm::vec3& worldCoord) const;

        //! Calculates the area of the screen-space elliptical projection of \a sphere
        float calcScreenArea(const Sphere& sphere, const glm::vec2& screenSize) const;
        //! Calculates the screen-space elliptical projection of \a sphere, putting the results in \a outCenter, \a outAxisA and \a outAxisB
        void calcScreenProjection(const Sphere& sphere, const glm::vec2& screenSize, glm::vec2* outCenter, glm::vec2* outAxisA, glm::vec2* outAxisB) const;

    protected:
        Camera() = default;

        void calcMatrices() const;
        virtual void calcViewMatrix() const;
        virtual void calcInverseView() const;
        virtual void calcProjection() const = 0;
        virtual void calcInverseProjection() const;

        virtual Ray calcRay(const glm::vec2& uv, float imagePlaneAspectRatio) const = 0;

        void getClipCoordinates(float clipDist, float ratio, glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight) const;
        void dirtyViewCaches() { modelViewCached = inverseModelViewCached = false; }
        bool isViewCachesDirty() { return modelViewCached || inverseModelViewCached; }
        void dirtyProjectionCaches() { projectionCached = inverseProjectionCached = false; }
        bool isProjectionCachesDirty() { return projectionCached || inverseProjectionCached; }

        glm::vec3 eyePoint{ vec3::zero };
        glm::vec3 viewDirection{ vec3::forward };
        glm::quat orientation{ quat::identity };
        glm::vec3 worldUp{ vec3::up };

        float fov{ 35.0f }; // vertical field of view in degrees
        float aspectRatio{ 1.0f };
        float nearClip{ 0.1f };
        float farClip{ 1000.f };
        float pivotDistance{ 1.0f };

        //mutable glm::vec3 forwardVector;
        mutable glm::vec3 rightVector;
        mutable glm::vec3 upVector;

        mutable glm::mat4 projectionMatrix;
        mutable glm::mat4 inverseProjectionMatrix;
        mutable glm::mat4 viewMatrix;
        mutable glm::mat4 inverseModelViewMatrix;

        mutable bool projectionCached{ false };
        mutable bool inverseProjectionCached{ false };
        mutable bool modelViewCached{ false };
        mutable bool inverseModelViewCached{ false };

        //Frustum frustum;
        mutable float frustumLeft{ -1.0f };
        mutable float frustumRight{ 1.0f };
        mutable float frustumBottom{ -1.0f };
        mutable float frustumTop{ 1.0f };
    };
}
