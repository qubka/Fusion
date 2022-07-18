#include "perspective_camera.hpp"

using namespace fe;

PerspectiveCamera::PerspectiveCamera() {
    lookAt({28, 21, 28}, vec3::zero, vec3::up);
    setPerspective(35, 1.3333f, 0.1f, 1000.0f);
}

PerspectiveCamera::PerspectiveCamera(const glm::vec2& size, float fov) {
    float eyeX = size.x / 2.0f;
    float eyeY = size.y / 2.0f;
    float halfFov = 3.14159f * fov / 360.0f;
    float theTan = std::tan(halfFov);
    float dist = eyeY / theTan;
    float nearDist = dist / 10.0f;    // near / far clip plane
    float farDist = dist * 10.0f;
    float aspect = size.x / size.y;

    setPerspective(fov, aspect, nearDist, farDist);
    lookAt({eyeX, eyeY, dist}, {eyeX, eyeY, 0.0f});
}

PerspectiveCamera::PerspectiveCamera(const glm::vec2& size, float fov, float near, float far) {
    float halfFov, theTan, aspect;
    float eyeX = size.x / 2.0f;
    float eyeY = size.y / 2.0f;
    halfFov = 3.14159f * fov / 360.0f;
    theTan = std::tan(halfFov);
    float dist = eyeY / theTan;
    aspect = size.x / size.y;

    setPerspective(fov, aspect, near, far);
    lookAt({eyeX, eyeY, dist}, {eyeX, eyeY, 0.0f});
}

void PerspectiveCamera::setPerspective(float fov, float aspect, float near, float far) {
    if (!isProjectionCachesDirty() && fovDegrees == fov && aspectRatio == aspect && nearClip == near && farClip == far)
        return;

    fovDegrees = fov;
    aspectRatio = aspect;
    nearClip = near;
    farClip = far;

    dirtyProjectionCaches();
}

Ray PerspectiveCamera::calcRay(const glm::vec2& uv, float imagePlaneAspectRatio) const {
    calcMatrices();

    float s = (uv.s - 0.5f + 0.5f * lensShift.x) * imagePlaneAspectRatio;
    float t = (uv.t - 0.5f + 0.5f * lensShift.y);
    float viewDistance = imagePlaneAspectRatio / fabsf(frustumRight - frustumLeft) * nearClip;

    return { eyePoint, glm::normalize(rightVector * s + upVector * t - (viewDirection * viewDistance)) };
}

void PerspectiveCamera::calcProjection() const {
    frustumTop = nearClip * std::tan(glm::radians(fovDegrees) * 0.5f);
    frustumBottom = -frustumTop;
    frustumRight = frustumTop * aspectRatio;
    frustumLeft = -frustumRight;

    // perform lens shift
    if (lensShift.y != 0.0f) {
        frustumTop = glm::lerp(0.0f, 2.0f * frustumTop, 0.5f + 0.5f * lensShift.y);
        frustumBottom = glm::lerp(2.0f * frustumBottom, 0.0f, 0.5f + 0.5f * lensShift.y);
    }

    if (lensShift.x != 0.0f) {
        frustumRight = glm::lerp(2.0f * frustumRight, 0.0f, 0.5f - 0.5f * lensShift.x);
        frustumLeft = glm::lerp(0.0f, 2.0f * frustumLeft, 0.5f - 0.5f * lensShift.x);
    }

    projectionMatrix = glm::frustum(frustumLeft, frustumRight, frustumBottom, frustumTop, nearClip, farClip);

    projectionCached = true;
    inverseProjectionCached = false;
}

void PerspectiveCamera::setLensShift(const glm::vec2& shift) {
    if (lensShift == shift)
        return;

    lensShift = shift;

    dirtyProjectionCaches();
}

PerspectiveCamera PerspectiveCamera::calcFraming(const Sphere& worldSpaceSphere) const {
    PerspectiveCamera result = *this;
    float xDistance = worldSpaceSphere.getRadius() / sinf(glm::radians(getFovHorizontal() * 0.5f));
    float yDistance = worldSpaceSphere.getRadius() / sinf(glm::radians(getFov() * 0.5f));
    result.setEyePoint(worldSpaceSphere.getCenter() + result.viewDirection * std::max(xDistance, yDistance));
    result.pivotDistance = glm::distance(result.eyePoint, worldSpaceSphere.getCenter());
    return result;
}

PerspectiveCamera PerspectiveCamera::subdivide(const glm::vec2& gridSize, const glm::vec2& gridIndex) const {
    PerspectiveCamera result = *this;
    result.setAspectRatio(getAspectRatio() * gridSize.x / gridSize.y);
    result.setLensShift(vec2::one - gridSize + 2.0f * gridIndex);
    return result;
}