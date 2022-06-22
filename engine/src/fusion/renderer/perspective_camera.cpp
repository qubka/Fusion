#include "perspective_camera.hpp"

#include "fusion/utils/math.hpp"

using namespace fe;

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far) : Camera{}
    , fovy{fov}
    , aspect{aspect}
    , near{near}
    , far{far}
{
    assert(far > near && "Far cannot be less then near");
    updateProjection();
    updateView();
}

void PerspectiveCamera::setPerspective(float fov, float aspectRatio, float nearClip, float farClip) {
    fovy = fov;
    aspect = aspectRatio;
    near = nearClip;
    far = farClip;
    updateProjection();
}

void PerspectiveCamera::updateDirs() {
    forward = rotation * vec3::forward;
    right = rotation * vec3::right;
    up = rotation * vec3::up;
}

void PerspectiveCamera::updateView() {
    if (!dirty)
        return;

    viewMatrix = glm::lookAt(position, position + forward, up);
    viewProjectionMatrix = projectionMatrix * viewMatrix;

    transformMatrix = glm::mat4{ //translate
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { position, 1.0f }
    } * glm::mat4_cast(rotation); //rotate;

    dirty = false;
}

void PerspectiveCamera::updateProjection() {
    projectionMatrix = glm::perspective(glm::radians(fovy), aspect, near, far);
    //projectionMatrix[1][1] *= -1;
    dirty = true;
}