#include "transform.hpp"

using namespace fe;

Transform::Transform(const glm::mat4& matrix) {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, localScale, localOrientation, localPosition, skew, perspective);
    localMatrix = matrix;
    worldMatrix = matrix;
}

Transform::Transform(const glm::vec3& position) {
    setLocalPosition(position);
}

void Transform::calcMatrices() {
    localMatrix =
    glm::mat4{ //translate
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { localPosition, 1.0f }
    }
    * glm::mat4_cast(localOrientation) //rotate
    * glm::mat4{ //scale
        { localScale.x, 0.0f, 0.0f, 0.0f },
        { 0.0f, localScale.y, 0.0f, 0.0f },
        { 0.0f, 0.0f, localScale.z, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f }
    };

    worldMatrix = parentMatrix * localMatrix;
    dirty = false;
    updated = true;
}

void Transform::applyTransform() {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(localMatrix, localScale, localOrientation, localPosition, skew, perspective);
    dirty = false;
    updated = true;
}

const glm::mat4& Transform::getWorldMatrix() {
    if (dirty)
        calcMatrices();
    return worldMatrix;
}

const glm::mat4& Transform::getLocalMatrix() {
    if (dirty)
        calcMatrices();
    return localMatrix;
}

void Transform::setWorldMatrix(const glm::mat4& mat) {
    if (dirty)
        calcMatrices();
    parentMatrix = mat;
    worldMatrix = parentMatrix * localMatrix;
}

void Transform::setLocalTransform(const glm::mat4& localMat) {
    localMatrix = localMat;
    updated = true;

    applyTransform();

    worldMatrix = parentMatrix * localMatrix;
}

void Transform::setLocalPosition(const glm::vec3& localPos) {
    localPosition = localPos;
    dirty = true;
}

void Transform::setLocalScale(const glm::vec3& scale) {
    localScale = scale;
    dirty = true;
}

void Transform::setLocalOrientation(const glm::quat& rotation) {
    localOrientation = rotation;
    dirty = true;
}

void Transform::setLocalOrientation(const glm::vec3& axis, float angle) {
    localOrientation = glm::angleAxis(angle, axis);
    dirty = true;
}

glm::vec3 Transform::getWorldUpDirection() const {
    const glm::mat4& m = worldMatrix;
    return { -m[1][0], -m[1][1], -m[1][2] };
}

glm::vec3 Transform::getLocalUpDirection() const {
    const glm::mat4& m = localMatrix;
    return { m[1][0], m[1][1], m[1][2] };
}

glm::vec3 Transform::getWorldDownDirection() const {
    const glm::mat4& m = worldMatrix;
    return { m[1][0], m[1][1], m[1][2] };
}

glm::vec3 Transform::getLocalDownDirection() const {
    const glm::mat4& m = localMatrix;
    return { -m[1][0], -m[1][1], -m[1][2] };
}

glm::vec3 Transform::getWorldLeftDirection() const {
    const glm::mat4& m = worldMatrix;
    return { -m[0][0], -m[0][1], -m[0][2] };
}

glm::vec3 Transform::getLocalLeftDirection() const {
    const glm::mat4& m = localMatrix;
    return { -m[0][0], -m[0][1], -m[0][2] };
}

glm::vec3 Transform::getWorldRightDirection() const {
    const glm::mat4& m = worldMatrix;
    return { m[0][0], m[0][1], m[0][2] };
}

glm::vec3 Transform::getLocalRightDirection() const {
    const glm::mat4& m = localMatrix;
    return { m[0][0], m[0][1], m[0][2] };
}

glm::vec3 Transform::getWorldForwardDirection() const {
    const glm::mat4& m = worldMatrix;
    return { -m[2][0], -m[2][1], -m[2][2] };
}

glm::vec3 Transform::getLocalForwardDirection() const {
    const glm::mat4& m = localMatrix;
    return { -m[2][0], -m[2][1], -m[2][2] };
}

glm::vec3 Transform::getWorldBackDirection() const {
    const glm::mat4& m = worldMatrix;
    return { m[2][0], m[2][1], m[2][2] };
}

glm::vec3 Transform::getLocalBackDirection() const {
    const glm::mat4& m = localMatrix;
    return { m[2][0], m[2][1], m[2][2] };
}

void Transform::translateLocal(const glm::vec3& translation) {
    localPosition += translation;
    dirty = true;
}

void Transform::rotate(glm::quat rotation, Space space) {
    rotation = glm::normalize(rotation);

    switch (space) {
        case Space::Self:
            localOrientation = localOrientation * rotation;
            break;
        case Space::Parent:
            localOrientation = rotation * localOrientation;
            break;
        case Space::World: {
            glm::quat worldOrientation{ getWorldOrientation() };
            localOrientation = localOrientation * glm::inverse(worldOrientation) * rotation * worldOrientation;
            break;
        }
        default:
            break;
    }

    dirty = true;
}

void Transform::rotate(const glm::vec3& axis, float angle, Space space) {
    rotate(glm::angleAxis(angle, axis), space);
}

void Transform::lookAt(glm::vec3 target, glm::vec3 up) {
    glm::mat4 parentInv{ glm::inverse(parentMatrix) };
    target = parentInv * glm::vec4{target, 1}; // vec4 -> vec3
    up = parentInv * glm::vec4{up, 0}; // vec4 -> vec3
    glm::mat4 lookAtMatrix{ glm::inverse(glm::lookAt(localPosition, target, up)) };
    setLocalOrientation(glm::quat_cast(lookAtMatrix));
}

void Transform::scaleLocal(const glm::vec3& scale) {
    localScale *= scale;
    dirty = true;
}
glm::vec3 Transform::transformPoint(const glm::vec3& point) {
    return localMatrix * glm::vec4{point, 1};
}

glm::vec3 Transform::transformDirection(const glm::vec3& direction) {
    return localMatrix * glm::vec4{direction, 0};
}