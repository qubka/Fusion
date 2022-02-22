#include "orthographic_camera.hpp"

using namespace fe;

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) : Camera{} {
    setOrthographic(left, right, bottom, top);
    updateView();
}

void OrthographicCamera::setOrthographic(float left, float right, float bottom, float top) {
    projectionMatrix = glm::ortho(left, right, bottom, top);
    isDirty = true;
}

void OrthographicCamera::updateView() {
    if (!isDirty)
        return;

    glm::mat4 m{1.0f};
    glm::mat4 transform {glm::translate(m, position) *
                         glm::rotate(m, glm::radians(rotation), vec3::up)};

    viewMatrix = glm::inverse(transform);
    viewProjectionMatrix = projectionMatrix * viewMatrix;

    isDirty = false;
}
