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

void PerspectiveCamera::setViewport(int width, int height) {
    viewportWidth = static_cast<float>(width);
    viewportHeight = static_cast<float>(height);
    setAspect(viewportWidth / viewportHeight);
}

void PerspectiveCamera::setPerspective(float aspectRatio, float fov, float nearClip, float farClip) {
    aspect = aspectRatio;
    fovy = fov;
    near = nearClip;
    far = farClip;
    updateProjection();
}

/// @link https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/

glm::vec3 PerspectiveCamera::calcForward() const {
#ifdef GLFW_INCLUDE_VULKAN
    return rotation * vec3::forward;
#else
    return rotation * vec3::back;
#endif
}

glm::vec3 PerspectiveCamera::calcUp() const {
#ifdef GLFW_INCLUDE_VULKAN
    return rotation * vec3::down;
#else
    return rotation * vec3::up;
#endif
}

glm::vec3 PerspectiveCamera::calcRight() const {
    return rotation * vec3::right;
}

void PerspectiveCamera::updateView() {
    if (!isDirty)
        return;

    forward = calcForward();
    right = calcRight();
    up = calcUp();

    viewMatrix = glm::lookAt(position, position + forward, up);
    viewProjectionMatrix = projectionMatrix * viewMatrix;

    isDirty = false;
}

void PerspectiveCamera::updateProjection() {
    projectionMatrix = glm::perspective(glm::radians(fovy), aspect, near, far);
    isDirty = true;
}
