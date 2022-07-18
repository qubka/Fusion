#include "scene_camera.hpp"

using namespace fe;

SceneCamera::SceneCamera() {
    recalculateProjection();
}

void SceneCamera::setPerspective(float fov, float near, float far) {
    projectionType = ProjectionType::Perspective;
    fovDegrees = fov;
    near = nearClip;
    far = farClip;
    recalculateProjection();
}

void SceneCamera::setOrthographic(float size, float near, float far) {
    projectionType = ProjectionType::Orthographic;
    fovDegrees = size;
    nearClip = near;
    farClip = far;
    recalculateProjection();
}

void SceneCamera::recalculateProjection() {
    switch (projectionType) {
        case ProjectionType::Perspective:
            projectionMatrix = glm::perspective(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);
            break;
        case ProjectionType::Orthographic:
            float& size = fovDegrees;
            float orthoLeft = -size * aspectRatio * 0.5f;
            float orthoRight = size * aspectRatio * 0.5f;
            float orthoBottom = -size * 0.5f;
            float orthoTop = size * 0.5f;
            projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, nearClip, farClip);
            break;
    }
}
