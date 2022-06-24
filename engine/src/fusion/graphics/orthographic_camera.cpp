#include "orthographic_camera.hpp"

#include "fusion/utils/glm_extention.hpp"

using namespace fe;

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) : Camera{} {
    setOrthographic(left, right, bottom, top);
    updateView();
}

void OrthographicCamera::setOrthographic(float left, float right, float bottom, float top) {
    projectionMatrix = glm::ortho(left, right, bottom, top);
    dirty = true;
}

void OrthographicCamera::updateView() {
    if (!dirty)
        return;

    transformMatrix = { //translate
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { position, 1.0f }
    };
    transformMatrix = glm::rotate(transformMatrix, glm::radians(rotation), vec3::up); //rotate

    viewMatrix = glm::inverse(transformMatrix);
    viewProjectionMatrix = projectionMatrix * viewMatrix;

    dirty = false;
}
