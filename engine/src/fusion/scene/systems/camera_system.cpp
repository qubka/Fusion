#include "camera_system.hpp"

using namespace fe;

CameraSystem::CameraSystem(entt::registry& registry) : System{registry} {

}

CameraSystem::~CameraSystem() {

}

void CameraSystem::onUpdate() {
    auto view = registry.view<TransformComponent, CameraComponent>();

    for (const auto& [entity, transform, camera] : view.each()) {
        camera.setEyePoint(transform.getWorldPosition());
        camera.setOrientation(transform.getWorldOrientation());
    }
}
