#include "scene.hpp"
#include "system.hpp"
#include "components.hpp"

#include "fusion/utils/string.hpp"
#include "fusion/filesystem/file_system.hpp"
#include "fusion/filesystem/virtual_file_system.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

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

void Scene::onStart() {
    systems.each([&](auto system) {
        if (system->isEnabled())
            system->onStart();
    });

    started = true;

    LOG_DEBUG << "Scene : " << name << " created first time";
}

void Scene::onUpdate() {
    if (!runtime)
        return;

    systems.each([&](auto system) {
        if (system->isEnabled())
            system->onUpdate();
    });
}

void Scene::onPlay() {
    runtime = true;

    systems.each([&](auto system) {
        if (system->isEnabled())
            system->onPlay();
    });

    LOG_DEBUG << "Scene : " << name << " started runtime";
}

void Scene::onStop() {
    systems.each([&](auto system) {
        if (system->isEnabled())
            system->onStop();
    });

    runtime = false;

    LOG_DEBUG << "Scene : " << name << " stopped runtime";
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

    /*clone<IdComponent>(newEntity, entity);
    clone<TagComponent>(newEntity, entity);
    // TODO: Clone children structure
    //clone<RelationshipComponent>(newEntity, entity);
    clone<TransformComponent>(newEntity, entity);
    clone<CameraComponent>(newEntity, entity);
    clone<MeshComponent>(newEntity, entity);
    clone<PointLightComponent>(newEntity, entity);
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

void Scene::serialise(bool binary) {
    if (binary) {
        auto filepath = VirtualFileSystem::Get()->resolvePhysicalPath("Scenes"_p / (name + ".bin"));
        std::ofstream os{filepath, std::ios::binary};
        {
            // output finishes flushing its contents when it goes out of scope
            cereal::BinaryOutputArchive output{os};
            output(*this);
            entt::snapshot{ registry }.entities(output).component<ALL_COMPONENTS>(output);
        }

        LOG_INFO << "Serialise scene as binary: " << filepath;
    } else {
        auto filepath = VirtualFileSystem::Get()->resolvePhysicalPath("Scenes"_p / (name + ".fsn"));
        std::stringstream ss;
        {
            // output finishes flushing its contents when it goes out of scope
            cereal::JSONOutputArchive output{ss};
            output(*this);
            entt::snapshot{ registry }.entities(output).component<ALL_COMPONENTS>(output);
        }

        FileSystem::WriteText(filepath, ss.str());

        LOG_INFO << "Serialise scene as JSON: " << filepath;
    }
}

void Scene::deserialise(bool binary) {
    if (binary) {
        auto filepath = VirtualFileSystem::Get()->resolvePhysicalPath("Scenes"_p / (name + ".bin"));

        if (!FileSystem::ExistsInPath(filepath)) {
            LOG_ERROR << "No saved scene file found: " << filepath;
            return;
        }

        try {
            std::ifstream is{filepath, std::ios::binary};
            cereal::BinaryInputArchive input{is};
            input(*this);
            entt::snapshot_loader{ registry }.entities(input).component<ALL_COMPONENTS>(input);
        }
        catch (...) {
            LOG_ERROR << "Failed to load scene: " << filepath;
        }

        LOG_INFO << "Deserialise scene as binary: " << filepath;
    } else {
        auto filepath = VirtualFileSystem::Get()->resolvePhysicalPath("Scenes"_p / (name + ".fsn"));

        if (!FileSystem::ExistsInPath(filepath)) {
            LOG_ERROR << "No saved scene file found: " << filepath;
            return;
        }
        try {
            std::string data = FileSystem::ReadText(filepath);
            std::istringstream is;
            is.str(data);
            cereal::JSONInputArchive input{is};
            input(*this);

            entt::snapshot_loader{ registry }.entities(input).component<ALL_COMPONENTS>(input);
        }
        catch (...) {
            LOG_ERROR << "Failed to load scene: " << filepath;
        }

        LOG_INFO << "Deserialise scene as JSON: " << filepath;
    }
}