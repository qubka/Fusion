#include "scene_camera.hpp"

using namespace fe;

SceneCamera::SceneCamera() {
    recalculateProjection();
}

void SceneCamera::setPerspective(float fov, float near, float far) {
    projectionType = ProjectionType::Perspective;
    fov = fov;
    near = nearClip;
    far = farClip;
    recalculateProjection();
}

void SceneCamera::setOrthographic(float size, float near, float far) {
    projectionType = ProjectionType::Orthographic;
    fov = size;
    nearClip = near;
    farClip = far;
    recalculateProjection();
}

void SceneCamera::recalculateProjection() {
    switch (projectionType) {
        case ProjectionType::Perspective:
            projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
            break;
        case ProjectionType::Orthographic:
            float orthoLeft = -fov * aspectRatio * 0.5f;
            float orthoRight = fov * aspectRatio * 0.5f;
            float orthoBottom = -fov * 0.5f;
            float orthoTop = fov * 0.5f;
            projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, nearClip, farClip);
            break;
    }
}
