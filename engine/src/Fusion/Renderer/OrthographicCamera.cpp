#include "OrthographicCamera.hpp"

using namespace Fusion;

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) : Camera() {
    setProjection(left, right, bottom, top);
    updateViewMatrix();
}

void OrthographicCamera::setProjection(float left, float right, float bottom, float top) {
    projectionMatrix = glm::ortho(left, right, bottom, top);
    isDirty = true;
}

void OrthographicCamera::setPosition(const glm::vec3& pos) {
    position = pos;
    isDirty = true;
}

void OrthographicCamera::setRotation(float rot) {
    rotation = rot;
    isDirty = true;
}

void OrthographicCamera::setPositionAndRotation(const glm::vec3& pos, float rot) {
    position = pos;
    rotation = rot;
    isDirty = true;
}

void OrthographicCamera::updateViewMatrix() {
    if (!isDirty)
        return;

    glm::mat4 m{1};
    glm::mat4 transform = glm::translate(m, position) *
#ifdef GLFW_INCLUDE_VULKAN
            glm::rotate(m, glm::radians(rotation), vec3::down);
#else
            glm::rotate(m, glm::radians(rotation), vec3::up);
#endif
    viewMatrix = glm::inverse(transform);
    viewProjectionMatrix = projectionMatrix * viewMatrix;

    isDirty = false;
}
