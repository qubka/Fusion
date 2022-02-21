#include "perspective_camera.hpp"

using namespace Fusion;

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far) : Camera{},
    fovy{fov},
    aspect{aspect},
    near{near},
    far{far}
{
    assert(far > near && "Far cannot be less then near");
    updateProjection();
    updateView();
}

void PerspectiveCamera::setViewport(uint32_t width, uint32_t height) {
    viewportWidth = static_cast<float>(width);
    viewportHeight = static_cast<float>(height);
    setAspect(viewportWidth / viewportHeight);
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
    if (!isDirty)
        return;

    viewMatrix = glm::lookAt(position, position + forward, up);
    viewProjectionMatrix = projectionMatrix * viewMatrix;

    isDirty = false;
}

void PerspectiveCamera::updateProjection() {
    projectionMatrix = glm::perspective(glm::radians(fovy), aspect, near, far);
    isDirty = true;
}