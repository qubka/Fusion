#include "PerspectiveCamera.hpp"

#include "Fusion/Geometry/Ray.hpp"
#include "Fusion/Core/Window.hpp"
#include "Fusion/Events/WindowEvents.hpp"

using namespace Fusion;

PerspectiveCamera::PerspectiveCamera(Window& window, float fov, float near, float far) : Camera(),
    window{window},
    fovy{fov},
    near{near},
    far{far}
{
    FS_CORE_ASSERT(far > near, "Far cannot be less then near");
    updateViewMatrix();
    window.bus().subscribe(this, &PerspectiveCamera::onWindowResize);
}

PerspectiveCamera::~PerspectiveCamera() {
    window.bus().destroy(this, &PerspectiveCamera::onWindowResize);
}

void PerspectiveCamera::setPosition(const glm::vec3& pos) {
    position = pos;
    isDirty = true;
}

void PerspectiveCamera::setRotation(const glm::quat& rot) {
    rotation = rot;
    isDirty = true;
}

void PerspectiveCamera::setPositionAndRotation(const glm::vec3 &pos, const glm::quat &rot) {
    position = pos;
    rotation = rot;
    isDirty = true;
}

void PerspectiveCamera::setFov(float fov) {
    fovy = fov;
    isDirty = true;
}

void PerspectiveCamera::setFarClip(float zFar) {
    far = zFar;
    isDirty = true;
}

void PerspectiveCamera::setNearClip(float zNear) {
    near = zNear;
    isDirty = true;
}

/// @link https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/

glm::vec3 PerspectiveCamera::calcForward() const {
#ifdef GLFW_INCLUDE_VULKAN
    return rotation * vec3::forward;
#else
    return rotation * vec3::back;
#endif
}

glm::vec3 PerspectiveCamera::calcUp() const {
#ifdef GLFW_INCLUDE_VULKAN
    return rotation * vec3::down;
#else
    return rotation * vec3::up;
#endif
}

glm::vec3 PerspectiveCamera::calcRight() const {
    return rotation * vec3::right;
}

void PerspectiveCamera::updateViewMatrix() {
    if (!isDirty)
        return;

    forward = calcForward();
    right = calcRight();
    up = calcUp();

    viewMatrix = glm::lookAt(position, position + forward, up);
    projectionMatrix = glm::perspective(glm::radians(fovy), window.getAspect(), near, far);
    viewProjectionMatrix = projectionMatrix * viewMatrix;

    isDirty = false;
}

/// @link https://antongerdelan.net/opengl/raycasting.html
Ray PerspectiveCamera::screenPointToRay(const glm::vec2& pos) const {
    float mouseX = 2 * pos.x / static_cast<float>(window.getWidth() - 1);
    float mouseY = 2 * pos.y / static_cast<float>(window.getHeight() - 1);

#ifdef GLM_FORCE_DEPTH_ZERO_TO_ONE
    glm::vec4 screenPos(mouseX, -mouseY, 0, 1);
#else
    glm::vec4 screenPos(mouseX, -mouseY, -1, 1);
#endif
    glm::vec4 eyeRay = glm::inverse(projectionMatrix) * screenPos;
    eyeRay.z = -1;
    eyeRay.w = 0;
    glm::vec4 worldRay = glm::inverse(viewMatrix) * eyeRay;

    return { position, glm::normalize(glm::vec3{worldRay}) };
}

/// @link https://discourse.libcinder.org/t/screen-to-world-coordinates/1014/2
glm::vec3 PerspectiveCamera::screenToWorldPoint(const glm::vec2& pos) const {
    return glm::unProject(glm::vec3{pos, 0}, viewMatrix, projectionMatrix, window.getViewport());
}

/** Global Events */

void PerspectiveCamera::onWindowResize(const WindowResizeEvent& event) {
    isDirty = true;
}