#include "stereo_camera.hpp"

using namespace fe;

glm::vec3 StereoCamera::getEyePointShifted() const {
    if (!isStereo)
        return eyePoint;

    if (isLeft)
        return eyePoint - (orientation * vec3::right) * (0.5f * eyeSeparation);
    else
        return eyePoint + (orientation * vec3::right) * (0.5f * eyeSeparation);
}

void StereoCamera::setConvergence(float distance, bool adjustEyeSeparation) {
    if (convergence == distance)
        return;

    convergence = distance;
    dirtyProjectionCaches();

    if (adjustEyeSeparation)
        eyeSeparation = convergence / 30.0f;
}

void StereoCamera::setEyeSeparation(float distance) {
    if (eyeSeparation == distance)
        return;

    eyeSeparation = distance;

    dirtyViewCaches();
    dirtyProjectionCaches();
}

const glm::mat4& StereoCamera::getProjectionMatrix() const {
    if (!projectionCached)
        calcProjection();

    if (!isStereo)
        return projectionMatrix;
    else if (isLeft)
        return projectionMatrixLeft;
    else
        return projectionMatrixRight;
}

const glm::mat4& StereoCamera::getInverseProjectionMatrix() const {
    if (!inverseProjectionCached)
        calcInverseProjection();

    if (!isStereo)
        return inverseProjectionMatrix;
    else if (isLeft)
        return inverseProjectionMatrixLeft;
    else
        return inverseProjectionMatrixRight;
}

const glm::mat4& StereoCamera::getViewMatrix() const {
    if (!modelViewCached)
        calcViewMatrix();

    if (!isStereo)
        return viewMatrix;
    else if (isLeft)
        return viewMatrixLeft;
    else
        return viewMatrixRight;
}

const glm::mat4& StereoCamera::getInverseViewMatrix() const {
    if (!inverseModelViewCached)
        calcInverseView();

    if (!isStereo)
        return inverseModelViewMatrix;
    else if (isLeft)
        return inverseModelViewMatrixLeft;
    else
        return inverseModelViewMatrixRight;
}

void StereoCamera::calcViewMatrix() const {
    // calculate default matrix first
    PerspectiveCamera::calcViewMatrix();

    viewMatrixLeft = viewMatrix;
    viewMatrixRight = viewMatrix;

    // calculate left matrix
    glm::vec3 eye {eyePoint - (orientation * vec3::right) * (0.5f * eyeSeparation)};
    glm::vec3 d {-dot(eye, rightVector), -dot(eye, upVector), -dot(eye, -viewDirection)};

    viewMatrixLeft[3][0] = d.x;
    viewMatrixLeft[3][1] = d.y;
    viewMatrixLeft[3][2] = d.z;

    // calculate right matrix
    eye = eyePoint + (orientation * vec3::right) * (0.5f * eyeSeparation);
    d = {-dot(eye, rightVector), -dot(eye, upVector), -dot(eye, -viewDirection)};

    viewMatrixRight[3][0] = d.x;
    viewMatrixRight[3][1] = d.y;
    viewMatrixRight[3][2] = d.z;

    modelViewCached = true;
    inverseModelViewCached = false;
}

void StereoCamera::calcInverseView() const {
    if (!modelViewCached)
        calcViewMatrix();

    inverseModelViewMatrix = glm::affineInverse(viewMatrix);
    inverseModelViewMatrixLeft = glm::affineInverse(viewMatrixLeft);
    inverseModelViewMatrixRight = glm::affineInverse(viewMatrixRight);

    inverseModelViewCached = true;
}

void StereoCamera::calcProjection() const {
    // calculate default matrices first
    PerspectiveCamera::calcProjection();

    projectionMatrixLeft = projectionMatrix;
    projectionMatrixRight = projectionMatrix;

    // calculate right matrices
    projectionMatrixRight[2][0] = (frustumRight + frustumLeft - eyeSeparation * (nearClip / convergence)) / (frustumRight - frustumLeft);
    projectionMatrixRight[3][0] = (frustumRight + frustumLeft - eyeSeparation * (nearClip / convergence)) / (2.0f * nearClip);

    projectionCached = true;
    inverseProjectionCached = false;
}

void StereoCamera::calcInverseProjection() const {
    // calculate default matrices first
    PerspectiveCamera::calcInverseProjection();

    inverseProjectionMatrixLeft = inverseProjectionMatrix;
    inverseProjectionMatrixRight = inverseProjectionMatrix;

    // calculate left matrices
    inverseProjectionMatrixLeft[2][0] = (frustumRight + frustumLeft + eyeSeparation * (nearClip / convergence)) / (frustumRight - frustumLeft);
    inverseProjectionMatrixLeft[3][0] = (frustumRight + frustumLeft + eyeSeparation * (nearClip / convergence)) / (2.0f * nearClip);

    inverseProjectionCached = true;
}

void StereoCamera::getShiftedClipCoordinates(float clipDist, float ratio, glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight) const {
    calcMatrices();

    const glm::vec3 shiftedEyePoint = getEyePointShifted();

    const float shift = 0.5f * eyeSeparation * (nearClip / convergence) * (isStereo ? (isLeft ? 1.0f : -1.0f) : 0.0f);
    const float left = frustumLeft + shift;
    const float right = frustumRight + shift;

    topLeft = shiftedEyePoint + clipDist * viewDirection + ratio * (frustumTop * upVector + left * rightVector);
    topRight = shiftedEyePoint + clipDist * viewDirection + ratio * (frustumTop * upVector + right * rightVector);
    bottomLeft = shiftedEyePoint + clipDist * viewDirection + ratio * (frustumBottom * upVector + left * rightVector);
    bottomRight = shiftedEyePoint + clipDist * viewDirection + ratio * (frustumBottom * upVector + right * rightVector);
}
