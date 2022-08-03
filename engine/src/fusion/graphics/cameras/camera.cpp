#include "camera.hpp"

using namespace fe;

Camera::Camera()
        : aspectRatio{1.333f}
        , nearClip{0.1f}
        , farClip{1000.0f}
        , fovDegrees{45.0f}
        , orthographic{false} {
    lookAt(glm::vec3{28, 21, 28}, vec3::zero, vec3::up);
}

Camera::Camera(float fov, float near, float far, float aspect)
        : aspectRatio{aspect}
        , fovDegrees{fov}
        , nearClip{near}
        , farClip{far}
        , orthographic{false}
        , scale{1.0f} {
    lookAt(glm::vec3{28, 21, 28}, vec3::zero, vec3::up);
};

Camera::Camera(const glm::vec3& position, float fov, float near, float far, float aspect)
        : aspectRatio{aspect}
        , fovDegrees{fov}
        , nearClip{near}
        , farClip{far}
        , orthographic{false}
        , scale{1.0f} {
    lookAt(position, vec3::zero, vec3::up);
}

Camera::Camera(float aspect, float scale)
        : aspectRatio{aspect}
        , scale{scale}
        , fovDegrees{60.0f}
        , nearClip{-10.0}
        , farClip{10.0f}
        , orthographic{true} {
    lookAt(glm::vec3{28, 21, 28}, vec3::zero, vec3::up);
}

Camera::Camera(float aspect, float near, float far)
        : aspectRatio{aspect}
        , scale{1.0f}
        , fovDegrees{60.0f}
        , nearClip{near}
        , farClip{far}
        , orthographic{true} {
    lookAt(glm::vec3{28, 21, 28}, vec3::zero, vec3::up);
}

bool Camera::setFov(float fov) {
    if (glm::epsilonEqual(fovDegrees, fov, FLT_EPSILON))
        return false;

    fovDegrees = fov;
    dirtyProjection();
    return true;
}

bool Camera::setFovHorizontal(float fov) {
    fov = glm::degrees(2.0f * std::atan(std::tan(glm::radians(fov) * 0.5f) / aspectRatio));
    if (glm::epsilonEqual(fovDegrees, fov, FLT_EPSILON))
        return false;

    fovDegrees = fov;
    dirtyProjection();
    return true;
}

bool Camera::setAspectRatio(float aspect) {
    if (glm::epsilonEqual(aspectRatio, aspect, FLT_EPSILON))
        return false;

    aspectRatio = aspect;
    dirtyProjection();
    return true;
}

bool Camera::setNearClip(float near) {
    if (glm::epsilonEqual(nearClip, near, FLT_EPSILON))
        return false;

    nearClip = near;
    dirtyProjection();
    return true;
}

bool Camera::setFarClip(float far) {
    if (glm::epsilonEqual(farClip, far, FLT_EPSILON))
        return false;

    farClip = far;
    dirtyProjection();
    return true;
}

bool Camera::setScale(float scalar) {
    if (glm::epsilonEqual(scale, scalar, FLT_EPSILON))
        return false;

    scale = scalar;

    if (orthographic)
        dirtyProjection();
    return true;
}

bool Camera::setEyePoint(const glm::vec3& point) {
    if (glm::all(glm::epsilonEqual(eyePoint, point, FLT_EPSILON)))
        return false;

    eyePoint = point;
    dirtyView();
    return true;
}

bool Camera::setViewDirection(glm::vec3 direction) {
    direction = glm::normalize(direction);
    if (glm::all(glm::epsilonEqual(viewDirection, direction, FLT_EPSILON)))
        return false;

    viewDirection = direction;
    orientation = glm::rotation(viewDirection, vec3::forward);
    dirtyView();
    return true;
}

bool Camera::setOrientation(glm::quat rotation) {
    rotation = glm::normalize(rotation);
    if (glm::all(glm::epsilonEqual(orientation, rotation, FLT_EPSILON)))
        return false;

    orientation = rotation;
    viewDirection = orientation * vec3::forward;
    dirtyView();
    return true;
}

bool Camera::setWorldUp(glm::vec3 up) {
    up = glm::normalize(up);
    if (glm::all(glm::epsilonEqual(worldUp, up, FLT_EPSILON)))
        return false;

    worldUp = up;
    orientation = glm::toQuat(glm::alignZAxisWithTarget(viewDirection, worldUp));
    dirtyView();
    return true;
}

bool Camera::setOrthographic(bool flag) {
    if (orthographic == flag)
        return false;

    orthographic = flag;

    //dirtyView();
    dirtyProjection();
    return true;
}

bool Camera::lookAt(glm::vec3 target) {
    target -= eyePoint; // ray from target to eye

    glm::vec3 direction = glm::normalize(target);
    if (glm::all(glm::epsilonEqual(viewDirection, direction, FLT_EPSILON)))
        return false;

    viewDirection = direction;
    orientation = glm::toQuat(alignZAxisWithTarget(viewDirection, worldUp));
    pivotDistance = glm::length(target);

    dirtyView();
    return true;
}

bool Camera::lookAt(const glm::vec3& point, glm::vec3 target) {
    target -= point; // ray from target to eye

    glm::vec3 direction = glm::normalize(target);
    if (glm::all(glm::epsilonEqual(viewDirection, direction, FLT_EPSILON)) && glm::all(glm::epsilonEqual(eyePoint, point, FLT_EPSILON)))
        return false;

    eyePoint = point;
    viewDirection = direction;
    orientation = glm::quat(glm::toQuat(alignZAxisWithTarget(viewDirection, worldUp)));
    pivotDistance = glm::length(target);

    dirtyView();
    return true;
}

bool Camera::lookAt(const glm::vec3& point, glm::vec3 target, glm::vec3 up) {
    target -= point; // ray from target to eye
    up = glm::normalize(up);

    glm::vec3 direction = glm::normalize(target);
    if (glm::all(glm::epsilonEqual(viewDirection, direction, FLT_EPSILON)) && glm::all(glm::epsilonEqual(eyePoint, point, FLT_EPSILON)) && glm::all(glm::epsilonEqual(worldUp, up, FLT_EPSILON)))
        return false;

    eyePoint = point;
    worldUp = up;
    viewDirection = direction;
    orientation = glm::toQuat(alignZAxisWithTarget(viewDirection, worldUp));
    pivotDistance = glm::length(target);

    dirtyView();
    return true;
}

glm::vec2 Camera::worldToScreen(const glm::vec3& worldCoord, const glm::vec2& screenSize) const {
    if (screenSize.x == 0.0f || screenSize.y == 0.0f)
        return {};

    glm::vec4 eyeCoord{ getViewMatrix() * glm::vec4{worldCoord, 1} };
    glm::vec4 ndc{ getProjectionMatrix() * eyeCoord };
    ndc.x /= ndc.w;
    ndc.y /= ndc.w;
    //ndc.z /= ndc.w;
    return { (ndc.x + 1.0f) / 2.0f * screenSize.x, (1.0f - (ndc.y + 1.0f) / 2.0f) * screenSize.y };
}

glm::vec2 Camera::eyeToScreen(const glm::vec3& eyeCoord, const glm::vec2& screenSize) const {
    if (screenSize.x == 0.0f || screenSize.y == 0.0f)
        return {};

    glm::vec4 ndc{ getProjectionMatrix() * glm::vec4{eyeCoord, 1} };
    ndc.x /= ndc.w;
    ndc.y /= ndc.w;
    //ndc.z /= ndc.w;
    return { (ndc.x + 1.0f) / 2.0f * screenSize.x, (1.0f - (ndc.y + 1.0f) / 2.0f) * screenSize.y };
}

float Camera::worldToEyeDepth(const glm::vec3& worldCoord) const {
    const glm::mat4& m = getViewMatrix();
    return m[0][2] * worldCoord.x +
           m[1][2] * worldCoord.y +
           m[2][2] * worldCoord.z +
           m[3][2];
}

glm::vec3 Camera::worldToNdc(const glm::vec3& worldCoord) const {
    glm::vec4 eyeCoord{ getViewMatrix() * glm::vec4{worldCoord, 1} };
    glm::vec4 ndc{ getProjectionMatrix() * eyeCoord };
    return { ndc.x / ndc.w, ndc.y / ndc.w, ndc.z / ndc.w };
}

/// @link https://discourse.libcinder.org/t/screen-to-world-coordinates/1014/2
glm::vec3 Camera::screenToWorld(const glm::vec2& screenCoord, const glm::vec2& screenSize, bool flipY) const {
    if (screenSize.x == 0.0f || screenSize.y == 0.0f)
        return eyePoint;

    glm::vec4 viewport{ 0, flipY ? screenSize.y : 0, screenSize.x, flipY ? -screenSize.y : screenSize.y };
    return glm::unProject(glm::vec3{screenCoord, 0}, getViewMatrix(), getProjectionMatrix(), viewport);
}

/// @link https://antongerdelan.net/opengl/raycasting.html
Ray Camera::screenPointToRay(const glm::vec2& screenCoord, const glm::vec2& screenSize, bool flipY) const {
    if (screenSize.x <= 1.0f || screenSize.y <= 1.0f)
        return { eyePoint, viewDirection };

    glm::vec2 mouseNorm{ 2.0f * (screenCoord / screenSize) - 1.0f };

    if (flipY)
        mouseNorm.y *= -1.0f;

    /*glm::vec4 screenPos{ mouseNorm.x, mouseNorm.y, -1.0f, 1.0f };
    glm::vec4 eyeRay{ getInverseProjectionMatrix() * screenPos };
    eyeRay.z = -1;
    eyeRay.w = 0;
    glm::vec4 worldRay{ getInverseViewMatrix() * eyeRay };
    return { eyePoint, glm::normalize(glm::vec3{worldRay}) };*/

    auto invViewProjection = glm::inverse(getProjectionMatrix() * getViewMatrix());

    glm::vec4 near = invViewProjection * glm::vec4{mouseNorm.x, mouseNorm.y, 0.0f, 1.0f};
    near /= near.w;

    glm::vec4 far = invViewProjection * glm::vec4{mouseNorm.x, mouseNorm.y, 1.0f, 1.0f};
    far /= far.w;

    return { near, glm::normalize(glm::vec3{far} - glm::vec3{near}) };
}

void Camera::calcProjection() const {
    if (orthographic)
        projectionMatrix = glm::ortho(-aspectRatio * scale, aspectRatio * scale, -scale, scale, nearClip, farClip);
    else
        projectionMatrix = glm::perspective(glm::radians(fovDegrees), aspectRatio, nearClip, farClip);

    projectionDirty = false;
    inverseProjectionDirty = true;
}

void Camera::calcInverseProjection() const {
    if (projectionDirty)
        calcProjection();

    inverseProjectionMatrix = glm::inverse(projectionMatrix);
    inverseProjectionDirty = false;
}

void Camera::calcView() const {
    //forwardVector = orientation * vec3::forward; // same as viewDirection
    rightVector = orientation * vec3::right;
    upVector = orientation * vec3::up;

    viewMatrix = glm::lookAt(eyePoint, eyePoint + viewDirection, upVector);

    viewDirty = false;
    inverseViewDirty = true;
}

void Camera::calcInverseView() const {
    if (viewDirty)
        calcView();

    inverseViewMatrix = glm::inverse(viewMatrix);
    inverseViewDirty = false;
}

void Camera::calcFrustum() {
    if (projectionDirty)
        calcProjection();
    if (viewDirty)
        calcView();

    frustum.set(projectionMatrix * viewMatrix);
    frustumDirty = false;
}