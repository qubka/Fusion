#pragma once

#include "camera.hpp"

namespace fe {
    class PerspectiveCamera : public Camera {
    public:
        //! Creates a default camera with eyePoint at ( 28, 21, 28 ), looking at the origin, 35deg vertical field-of-view and a 1.333 aspect ratio.
        PerspectiveCamera();
        //! Constructs screen-aligned camera
        PerspectiveCamera(const glm::vec2& size, float fov);
        //! Constructs screen-aligned camera
        PerspectiveCamera(const glm::vec2& size, float fov, float near, float far);

        //! Configures the camera's projection according to the provided parameters.
        void setPerspective(float verticalFovDegrees, float aspect, float near, float far);

        /** Returns both the horizontal and vertical lens shift.
            A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
            A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
        glm::vec2 getLensShift() const { return lensShift; }
        /** Sets both the horizontal and vertical lens shift.
            A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
            A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
        void setLensShift(const glm::vec2& shift);

        //! Returns the horizontal lens shift. A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport.
        float getLensShiftHorizontal() const { return lensShift.x; }
        /** Sets the horizontal lens shift.
            A horizontal lens shift of 1 (-1) will shift the view right (left) by half the width of the viewport. */
        void setLensShiftHorizontal(float horizontal) { setLensShift({horizontal, lensShift.y}); }

        //! Returns the vertical lens shift. A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport.
        float getLensShiftVertical() const { return lensShift.y; }
        /** Sets the vertical lens shift.
            A vertical lens shift of 1 (-1) will shift the view up (down) by half the height of the viewport. */
        void setLensShiftVertical(float vertical) { setLensShift({lensShift.x, vertical}); }

        bool isOrthographic() const override { return false; }

        //! Returns a Camera whose eyePoint is positioned to exactly frame \a worldSpaceSphere but is equivalent in other parameters (including orientation). Sets the result's pivotDistance to be the distance to \a worldSpaceSphere's center.
        PerspectiveCamera calcFraming(const Sphere& worldSpaceSphere) const;
        //! Returns a subdivided portion of this camera's view frustrum as a new CameraPersp; useful for multi-gpu or tiled-rendering for instance.
        PerspectiveCamera subdivide(const glm::vec2& gridSize, const glm::vec2& gridIndex) const;

    protected:
        glm::vec2 lensShift{ 0.0f };

        void calcProjection() const override;
        Ray calcRay(const glm::vec2& uv, float imagePlaneAspectRatio) const override;
    };
}
