#include "scene.hpp"
#include "system.hpp"
#include "components.hpp"

#include "fusion/utils/string.hpp"

using namespace fe;

Scene::Scene(std::string name) : name{std::move(name)} {

}

/*Scene::Scene(const Scene& other) {
    registry.assign(other.registry.data(), other.registry.data() + other.registry.size(), other.registry.released());

    clone<IdComponent>(other.registry);
    clone<TagComponent>(other.registry);
    clone<RelationshipComponent>(other.registry);
    clone<TransformComponent>(other.registry);
    clone<CameraComponent>(other.registry);
    clone<MeshComponent>(other.registry);
    clone<PointLightComponent>(other.registry);
    clone<DirectionalLightComponent>(other.registry);
    clone<ScriptComponent>(other.registry);
    clone<RigidBodyComponent>(other.registry);
    clone<BoxColliderComponent>(other.registry);
    clone<SphereColliderComponent>(other.registry);
    clone<CapsuleColliderComponent>(other.registry);
    clone<MeshColliderComponent>(other.registry);
    clone<PhysicsMaterialComponent>(other.registry);
    clone<MaterialComponent>(other.registry);
}*/

void Scene::onCreate() {
    systems.each([&](auto system) {
        if (system->isEnabled())
            system->onCreate();
    });
}

void Scene::onUpdate() {
    if (!runtime)
        return;

    systems.each([&](auto system) {
        if (system->isEnabled())
            system->onUpdate();
    });
}

void Scene::onStart() {
    runtime = true;

    systems.each([&](auto system) {
        if (system->isEnabled())
            system->onStart();
    });
}

void Scene::onStop() {
    systems.each([&](auto system) {
        if (system->isEnabled())
            system->onStop();
    });

    runtime = false;
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

    return entity;
}

void Scene::destroyEntity(entt::entity entity) {
    registry.destroy(entity);
}

entt::entity Scene::duplicateEntity(entt::entity entity) {
    auto newEntity = registry.create();
    
    clone<IdComponent>(newEntity, entity);
    clone<TagComponent>(newEntity, entity);
    // TODO: Clone children structure
    //clone<RelationshipComponent>(newEntity, entity);
    clone<TransformComponent>(newEntity, entity);
    clone<CameraComponent>(newEntity, entity);
    clone<MeshComponent>(newEntity, entity);
    /*clone<PointLightComponent>(newEntity, entity);
    clone<DirectionalLightComponent>(newEntity, entity);
    clone<ScriptComponent>(newEntity, entity);
    clone<RigidBodyComponent>(newEntity, entity);
    clone<BoxColliderComponent>(newEntity, entity);
    clone<SphereColliderComponent>(newEntity, entity);
    clone<CapsuleColliderComponent>(newEntity, entity);
    clone<MeshColliderComponent>(newEntity, entity);
    clone<PhysicsMaterialComponent>(newEntity, entity);
    clone<MaterialComponent>(newEntity, entity);*/
    
    return newEntity;
}

/*void Scene::serialise(fs::path filename, bool binary = false) {
    if (binary) {
        filename += ".bin";
        std::ofstream file{filename, std::ios::binary};
        {
            // output finishes flushing its contents when it goes out of scope
            cereal::BinaryOutputArchive output{ file };
            //output(*this);
            entt::snapshot{ registry }.entities(output).component<ALL_COMPONENTS>(output);
        }
        file.close();
    } else {
        filename += ".fsn";
        std::stringstream storage;
        {
            // output finishes flushing its contents when it goes out of scope
            cereal::JSONOutputArchive output{ storage };
            //output(*this);
            entt::snapshot{ registry }.entities(output).component<ALL_COMPONENTS>(output);
        }
        FileSystem::WriteTextFile(filename, storage.str());
    }
}

void Scene::deserialise(const fs::path& filename, bool binary = false) {

}*/