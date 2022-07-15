#include "camera.hpp"

#include "fusion/utils/glm_extention.hpp"

using namespace fe;

void Camera::setFov(float verticalFov) {
    if (glm::epsilonEqual(fov, verticalFov, FLT_EPSILON))
        return;

    fov = verticalFov;
    dirtyProjectionCaches();
}

void Camera::setFovHorizontal(float horizontalFov) {
    horizontalFov = glm::degrees(2.0f * std::atan(std::tan(glm::radians(horizontalFov) * 0.5f) / aspectRatio));
    if (glm::epsilonEqual(fov, horizontalFov, FLT_EPSILON))
        return;

    fov = horizontalFov;
    dirtyProjectionCaches();
}

void Camera::setAspectRatio(float aspect) {
    if (glm::epsilonEqual(aspectRatio, aspect, FLT_EPSILON))
        return;

    aspectRatio = aspect;
    dirtyProjectionCaches();
}

void Camera::setNearClip(float near) {
    if (glm::epsilonEqual(nearClip, near, FLT_EPSILON))
        return;

    nearClip = near;
    dirtyProjectionCaches();
}

void Camera::setFarClip(float far) {
    if (glm::epsilonEqual(farClip, far, FLT_EPSILON))
        return;

    farClip = far;
    dirtyProjectionCaches();
}

void Camera::setEyePoint(const glm::vec3& point) {
    if (glm::all(glm::epsilonEqual(eyePoint, point, FLT_EPSILON)))
        return;

    eyePoint = point;
    dirtyViewCaches();
}

void Camera::setViewDirection(glm::vec3 direction) {
    direction = glm::normalize(direction);
    if (glm::all(glm::epsilonEqual(viewDirection, direction, FLT_EPSILON)))
        return;

    viewDirection = direction;
    orientation = glm::rotation(viewDirection, vec3::forward);
    dirtyViewCaches();
}

void Camera::setOrientation(glm::quat rotation) {
    rotation = glm::normalize(rotation);
    if (glm::all(glm::epsilonEqual(orientation, rotation, FLT_EPSILON)))
        return;

    orientation = rotation;
    viewDirection = orientation * vec3::forward;
    dirtyViewCaches();
}

void Camera::setWorldUp(glm::vec3 up) {
    up = glm::normalize(up);
    if (glm::all(glm::epsilonEqual(worldUp, up, FLT_EPSILON)))
        return;

    worldUp = up;
    orientation = glm::toQuat(glm::alignZAxisWithTarget(viewDirection, worldUp));
    dirtyViewCaches();
}

/// @link http://paulbourke.net/miscellaneous/lens/
float Camera::getFocalLength() const {
    return 1.0f / (glm::tan(glm::radians(fov) * 0.5f) * 2.0f);
}

void Camera::lookAt(glm::vec3 target) {
    target -= eyePoint; // ray from target to eye

    glm::vec3 direction { glm::normalize(target) };
    if (glm::all(glm::epsilonEqual(viewDirection, direction, FLT_EPSILON)))
        return;

    viewDirection = direction;
    orientation = glm::toQuat(alignZAxisWithTarget(viewDirection, worldUp));
    pivotDistance = glm::length(target);

    dirtyViewCaches();
}

void Camera::lookAt(const glm::vec3& point, glm::vec3 target) {
    target -= eyePoint; // ray from target to eye

    glm::vec3 direction { glm::normalize(target) };
    if (glm::all(glm::epsilonEqual(viewDirection, direction, FLT_EPSILON)) && glm::all(glm::epsilonEqual(eyePoint, point, FLT_EPSILON)))
        return;

    eyePoint = point;
    viewDirection = direction;
    orientation = glm::quat(glm::toQuat(alignZAxisWithTarget(viewDirection, worldUp)));
    pivotDistance = glm::length(target);

    dirtyViewCaches();
}

void Camera::lookAt(const glm::vec3& point, glm::vec3 target, glm::vec3 up) {
    target -= eyePoint; // ray from target to eye
    up = glm::normalize(up);

    glm::vec3 direction { glm::normalize(target) };
    if (glm::all(glm::epsilonEqual(viewDirection, direction, FLT_EPSILON)) && glm::all(glm::epsilonEqual(eyePoint, point, FLT_EPSILON)) && glm::all(glm::epsilonEqual(worldUp, up, FLT_EPSILON)))
        return;

    eyePoint = point;
    worldUp = up;
    viewDirection = direction;
    orientation = glm::toQuat(alignZAxisWithTarget(viewDirection, worldUp));
    pivotDistance = glm::length(target);

    dirtyViewCaches();
}

void Camera::getFrustum(float& left, float& top, float& right, float& bottom, float& near, float& far) const {
    calcMatrices();

    left = frustumLeft;
    top = frustumTop;
    right = frustumRight;
    bottom = frustumBottom;
    near = nearClip;
    far = farClip;
}

void Camera::getBillboardVectors(glm::vec3& right, glm::vec3& up) const {
    right = glm::row(getViewMatrix(), 0);
    up = glm::row(getViewMatrix(), 1);
}

glm::vec2 Camera::worldToScreen(const glm::vec3& worldCoord, const glm::vec2& screenSize) const {
    if (screenSize.x == 0.0f || screenSize.y == 0.0f)
        return {};

    glm::vec4 eyeCoord{ getViewMatrix() * glm::vec4{worldCoord, 1} };
    glm::vec4 ndc{ getProjectionMatrix() * eyeCoord };
    ndc.x /= ndc.w;
    ndc.y /= ndc.w;
    ndc.z /= ndc.w;
    return { (ndc.x + 1.0f) / 2.0f * screenSize.x, (1.0f - (ndc.y + 1.0f) / 2.0f) * screenSize.y };
}

glm::vec2 Camera::eyeToScreen(const glm::vec3& eyeCoord, const glm::vec2& screenSize) const {
    if (screenSize.x == 0.0f || screenSize.y == 0.0f)
        return {};

    glm::vec4 ndc{ getProjectionMatrix() * glm::vec4{eyeCoord, 1} };
    ndc.x /= ndc.w;
    ndc.y /= ndc.w;
    ndc.z /= ndc.w;
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
    glm::vec4 eye{ getViewMatrix() * glm::vec4{worldCoord, 1} };
    glm::vec4 unproj{ getProjectionMatrix() * eye };
    return { unproj.x / unproj.w, unproj.y / unproj.w, unproj.z / unproj.w };
}

/// @link https://discourse.libcinder.org/t/screen-to-world-coordinates/1014/2
glm::vec3 Camera::screenToWorld(const glm::vec2& screenCoord, const glm::vec2& screenSize) const {
    if (screenSize.x == 0.0f || screenSize.y == 0.0f)
        return eyePoint;

    glm::vec4 viewport{ 0, 0, screenSize.x, screenSize.y };
    return glm::unProject(glm::vec3{screenCoord, 0}, getViewMatrix(), getProjectionMatrix(), viewport);
}

/// @link https://antongerdelan.net/opengl/raycasting.html
Ray Camera::screenPointToRay(const glm::vec2& screenCoord, const glm::vec2& screenSize) const {
    if (screenSize.x <= 1.0f || screenSize.y <= 1.0f)
        return { eyePoint, viewDirection };

    glm::vec2 mouseNorm{ 2.0f * screenCoord / (screenSize - 1.0f) };

#ifdef GLM_FORCE_DEPTH_ZERO_TO_ONE
    glm::vec4 screenPos{ mouseNorm.x, -mouseNorm.y, 0, 1 };
#else
    glm::vec4 screenPos{ mouseNorm.x, -mouseNorm.y, -1, 1 };
#endif

    glm::vec4 eyeRay{ getInverseProjectionMatrix() * screenPos };
    eyeRay.z = -1;
    eyeRay.w = 0;
    glm::vec4 worldRay{ getInverseViewMatrix() * eyeRay };
    return { eyePoint, glm::normalize(glm::vec3{worldRay}) };
}

float Camera::calcScreenArea(const Sphere& sphere, const glm::vec2& screenSize) const {
    Sphere camSpaceSphere{glm::vec3{getViewMatrix() * glm::vec4{sphere.getCenter(), 1}}, sphere.getRadius()};
    return camSpaceSphere.calcProjectedArea(getFocalLength(), screenSize);
}

void Camera::calcScreenProjection(const Sphere& sphere, const glm::vec2& screenSize, glm::vec2* outCenter, glm::vec2* outAxisA, glm::vec2* outAxisB) const {
    if (screenSize.x == 0.0f || screenSize.y == 0.0f)
        return;

    auto toScreenPixels = [=](glm::vec2 result, const glm::vec2& windowSize) {
        result.x *= 1 / (windowSize.x / windowSize.y);
        result += glm::vec2{0.5f};
        result *= windowSize;
        return result;
    };

    Sphere camSpaceSphere{glm::vec3{getViewMatrix() * glm::vec4{sphere.getCenter(), 1}}, sphere.getRadius()};
    glm::vec2 center, axisA, axisB;
    camSpaceSphere.calcProjection(getFocalLength(), &center, &axisA, &axisB);

    if (outCenter)
        *outCenter = toScreenPixels(center, screenSize);

    if (outAxisA)
        *outAxisA = toScreenPixels(center + axisA * 0.5f, screenSize) -
                    toScreenPixels(center - axisA * 0.5f, screenSize);
    if (outAxisB)
        *outAxisB = toScreenPixels(center + axisB * 0.5f, screenSize) -
                    toScreenPixels(center - axisB * 0.5f, screenSize);
}

void Camera::calcInverseProjection() const {
    if (!projectionCached)
        calcProjection();

    inverseProjectionMatrix = glm::inverse(projectionMatrix);
    inverseProjectionCached = true;
}

void Camera::calcMatrices() const {
    if (!modelViewCached)
        calcViewMatrix();
    if (!projectionCached)
        calcProjection();
}

void Camera::calcViewMatrix() const {
    //forwardVector = orientation * vec3::forward; // should be same as viewDirection
    rightVector = orientation * vec3::right;
    upVector = orientation * vec3::up;

    viewMatrix = glm::lookAt(eyePoint, eyePoint + viewDirection, upVector);

    modelViewCached = true;
    inverseModelViewCached = false;
}

void Camera::calcInverseView() const {
    if (!modelViewCached)
        calcViewMatrix();

    inverseModelViewMatrix = glm::inverse(viewMatrix);
    inverseModelViewCached = true;
}

void Camera::getClipCoordinates(float clipDist, float ratio, glm::vec3& topLeft, glm::vec3& topRight, glm::vec3& bottomLeft, glm::vec3& bottomRight) const {
    calcMatrices();

    topLeft = eyePoint - clipDist * viewDirection + ratio * (frustumTop * upVector) + ratio * (frustumLeft * rightVector);
    topRight = eyePoint - clipDist * viewDirection + ratio * (frustumTop * upVector) + ratio * (frustumRight * rightVector);
    bottomLeft = eyePoint - clipDist * viewDirection + ratio * (frustumBottom * upVector) + ratio * (frustumLeft * rightVector);
    bottomRight = eyePoint - clipDist * viewDirection + ratio * (frustumBottom * upVector) + ratio * (frustumRight * rightVector);
}
