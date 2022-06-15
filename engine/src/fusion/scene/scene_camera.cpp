#include "scene_camera.hpp"

using namespace fe;

SceneCamera::SceneCamera() {
    recalculateProjection();
}

void SceneCamera::setPerspective(float fov, float nearClip, float farClip) {
    projectionType = ProjectionType::Perspective;
    perspectiveFOV = fov;
    perspectiveNear = nearClip;
    perspectiveFar = farClip;
    recalculateProjection();
}

void SceneCamera::setOrthographic(float size, float nearClip, float farClip) {
    projectionType = ProjectionType::Orthographic;
    orthographicSize = size;
    orthographicNear = nearClip;
    orthographicFar = farClip;
    recalculateProjection();
}

void SceneCamera::recalculateProjection() {
    switch (projectionType) {
        case ProjectionType::Perspective:
            projectionMatrix = glm::perspective(glm::radians(perspectiveFOV), aspectRatio, perspectiveNear, perspectiveFar);
            //projectionMatrix[1][1] *= -1;
            break;
        case ProjectionType::Orthographic:
            float orthoLeft = -orthographicSize * aspectRatio * 0.5f;
            float orthoRight = orthographicSize * aspectRatio * 0.5f;
            float orthoBottom = -orthographicSize * 0.5f;
            float orthoTop = orthographicSize * 0.5f;

            projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthographicNear, orthographicFar);
            break;
    }
}
