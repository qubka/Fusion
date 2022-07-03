#include "scene.hpp"
#include "system.hpp"
#include "components.hpp"

#include "fusion/utils/string.hpp"

using namespace fe;

Scene::Scene(const Scene& other) {
    registry.assign(other.registry.data(), other.registry.data() + other.registry.size(), other.registry.released());

    clone<TagComponent>(other.registry);
    clone<RelationshipComponent>(other.registry);
    clone<TransformComponent>(other.registry);
    clone<CameraComponent>(other.registry);
    clone<MeshComponent>(other.registry);
    clone<PointLightComponent>(other.registry);
    clone<DirectionalLightComponent>(other.registry);
    clone<ScriptComponent>(other.registry);
    /*clone<RigidBodyComponent>(other.registry);
    clone<BoxColliderComponent>(other.registry);
    clone<SphereColliderComponent>(other.registry);
    clone<CapsuleColliderComponent>(other.registry);
    clone<MeshColliderComponent>(other.registry);
    clone<PhysicsMaterialComponent>(other.registry);
    clone<MaterialComponent>(other.registry);*/

    systems = other.systems;
}

void Scene::start() {
    systems.each([&](auto system) {
        if (system->isEnabled())
            system->create();
    });
}

void Scene::update() {
    systems.each([&](auto system) {
        if (system->isEnabled())
            system->update();
    });

    //camera->update();
}

void Scene::runtimeStart() {
    systems.each([&](auto system) {
        if (system->isEnabled())
            system->start();
    });

    active = true;
}

void Scene::runtimeStop() {
    systems.each([&](auto system) {
        if (system->isEnabled())
            system->stop();
    });

    active = false;
}

void Scene::clearSystems() {
    systems.clear();
}

void Scene::clearEntities() {
    registry.destroy(registry.data(), registry.data() + registry.size());
}

entt::entity Scene::createEntity(std::string name) {
    auto entity = registry.create();
    registry.emplace<TransformComponent>(entity);

    if (name.empty())
        name = "Empty Entity";

    size_t idx = 0;
    auto view = registry.view<const TagComponent>();
    for (auto [e, tag] : view.each()) {
        if (String::Contains(tag, name)) {
            idx++;
        }
    }
    if (idx > 0)
        name += " (" + std::to_string(idx) + ")";

    registry.emplace<TagComponent>(entity, name);
}

void Scene::destroyEntity(entt::entity entity) {
    registry.destroy(entity);
}

entt::entity Scene::duplicateEntity(entt::entity entity) {
    auto newEntity = registry.create();
    
    clone<TagComponent>(newEntity, entity);
    // TODO: Clone children structure
    //clone<RelationshipComponent>(newEntity, entity);
    clone<TransformComponent>(newEntity, entity);
    clone<CameraComponent>(newEntity, entity);
    clone<MeshComponent>(newEntity, entity);
    clone<PointLightComponent>(newEntity, entity);
    clone<DirectionalLightComponent>(newEntity, entity);
    clone<ScriptComponent>(newEntity, entity);
    /*clone<RigidBodyComponent>(newEntity, entity);
    clone<BoxColliderComponent>(newEntity, entity);
    clone<SphereColliderComponent>(newEntity, entity);
    clone<CapsuleColliderComponent>(newEntity, entity);
    clone<MeshColliderComponent>(newEntity, entity);
    clone<PhysicsMaterialComponent>(newEntity, entity);
    clone<MaterialComponent>(newEntity, entity);*/
    
    return newEntity;
}