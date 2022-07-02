#include "orthographic_camera.hpp"

using namespace fe;

OrthographicCamera::OrthographicCamera() {
    lookAt({28, 21, 28}, vec3::zero, vec3::up);
    setOrthographic(-1, 1, -1, 1, 0.1f, 1000.f);
}

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float near, float far) {
    setOrthographic(left, right, bottom, top, near, far);
}

void OrthographicCamera::setOrthographic(float left, float right, float bottom, float top, float near, float far) {
    if (!isProjectionCachesDirty() && frustumLeft == left && frustumRight == right && frustumTop == top && nearClip == near &&  farClip == far)
        return;

    frustumLeft = left;
    frustumRight = right;
    frustumTop = top;
    frustumBottom = bottom;
    nearClip = near;
    farClip = far;

    dirtyProjectionCaches();
}

void OrthographicCamera::calcProjection() const {
    projectionMatrix = glm::ortho(frustumLeft, frustumRight, frustumBottom, frustumTop, nearClip, farClip);

    projectionCached = true;
    inverseProjectionCached = false;
}

Ray OrthographicCamera::calcRay(const glm::vec2& uv, float imagePlaneAspectRatio) const {
    calcMatrices();

    float s = (uv.s - 0.5f) * imagePlaneAspectRatio;
    float t = (uv.t - 0.5f);

    return { eyePoint + rightVector * s * (frustumRight - frustumLeft) + upVector * t * (frustumTop - frustumBottom), viewDirection };
}