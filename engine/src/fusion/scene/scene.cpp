#include "scene.hpp"
#include "system.hpp"
#include "components.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/models/model.hpp"
#include "fusion/filesystem/file_system.hpp"
#include "fusion/scene/systems/hierarchy_system.hpp"
#include "fusion/scene/systems/camera_system.hpp"
#include "fusion/scene/systems/physics_system.hpp"

using namespace fe;

Scene::Scene(std::string_view name) : name{name} {
    addSystem<HierarchySystem>();
    addSystem<CameraSystem>();
    addSystem<PhysicsSystem>();
}

Scene::Scene(const Scene& other) : Scene{other.name} {
    registry.assign(other.registry.data(), other.registry.data() + other.registry.size(), other.registry.released());
    copyRegistry<ALL_COMPONENTS>(other.registry);
}

void Scene::onStart() {
    systems.each([&](auto system) {
        system->setEnabled(true);
    });

    LOG_DEBUG << "Scene : \"" << name << "\" created";
}

void Scene::onUpdate() {
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

    LOG_DEBUG << "Scene : \"" << name << "\" started runtime";
}

void Scene::onStop() {
    systems.each([&](auto system) {
        if (system->isEnabled())
            system->onStop();
    });

    runtime = false;

    LOG_DEBUG << "Scene : \"" << name << "\" stopped runtime";
}

void Scene::clearSystems() {
    systems.clear();
}

void Scene::clearEntities() {
    registry.destroy(registry.data(), registry.data() + registry.size());
}

const Camera* Scene::getCamera() const {
    auto view = registry.view<CameraComponent>();
    if (!view.empty()) {
        return static_cast<const Camera*>(&registry.get<CameraComponent>(view.front()));
    } else {
        return nullptr;
    }
}

entt::entity Scene::getCameraEntity() const {
    auto view = registry.view<CameraComponent>();
    if (!view.empty()) {
        return view.front();
    } else {
        return entt::null;
    }
}

entt::entity Scene::createEntity(std::string name) {
    auto entity = registry.create();
    registry.emplace<IdComponent>(entity);
    //registry.emplace<TransformComponent>(entity);

    if (name.empty())
        name = "Empty Entity";

    uint32_t i = 0;
    auto view = registry.view<NameComponent>();
    for (const auto& [e, str] : view.each()) {
        if (str.name.find(name) != std::string::npos) {
            i++;
        }
    }
    if (i > 0)
        name += " (" + std::to_string(i) + ")";

    registry.emplace<NameComponent>(entity, name);

    return entity;
}

bool Scene::destroyEntity(entt::entity entity) {
    // TODO: Children ?
    registry.destroy(entity);
    return true;
}

entt::entity Scene::duplicateEntity(entt::entity entity, entt::entity parent) {
    auto hierarchySystem = getSystem<HierarchySystem>();
    hierarchySystem->setEnabled(false);

    auto newEntity = registry.create();
    copyEntity<ALL_COMPONENTS>(newEntity, entity);

    if (auto hierarchyComponent = registry.try_get<HierarchyComponent>(newEntity)) {
        hierarchyComponent->children = 0;
        hierarchyComponent->first = entt::null;
        hierarchyComponent->parent = entt::null;
        hierarchyComponent->next = entt::null;
        hierarchyComponent->prev = entt::null;
    }

    auto children = hierarchySystem->getChildren(entity);

    for (const auto child : children)  {
        duplicateEntity(child, newEntity);
    }

    if (parent != entt::null)
        hierarchySystem->assignChild(parent, newEntity);

    hierarchySystem->setEnabled(true);
    
    return newEntity;
}

void Scene::serialise(fs::path filepath, bool binary) {
    if (binary) {
        if (filepath.empty())
            filepath = Engine::Get()->getApp()->getRootPath() / "assets/scenes" / (name + ".bin");

        std::ofstream os{filepath, std::ios::binary};
        {
            cereal::BinaryOutputArchive output{os};
            output(*this);
            entt::snapshot{ registry }.entities(output).component<ALL_COMPONENTS>(output);
        }

        LOG_INFO << "Serialise scene as binary: \"" << filepath << "\"";
    } else {
        if (filepath.empty())
            filepath = Engine::Get()->getApp()->getRootPath() / "assets/scenes" / (name + ".fsn");

        std::stringstream ss;
        {
            cereal::JSONOutputArchive output{ss};
            output(*this);
            entt::snapshot{ registry }.entities(output).component<ALL_COMPONENTS>(output);
        }

        FileSystem::WriteText(filepath, ss.str());

        LOG_INFO << "Serialise scene: \"" << filepath << "\"";
    }
}

void Scene::deserialise(fs::path filepath, bool binary) {
    systems.each([&](auto system) {
        system->setEnabled(false);
    });

    if (binary) {
        if (filepath.empty())
            filepath = Engine::Get()->getApp()->getRootPath() / "assets/scenes" / (name + ".bin");

        if (!fs::exists(filepath) || !fs::is_regular_file(filepath)) {
            LOG_ERROR << "No saved scene file found: \"" << filepath << "\"";
            return;
        }

        try {
            std::ifstream is{filepath, std::ios::binary};
            cereal::BinaryInputArchive input{is};
            input(*this);
            entt::snapshot_loader{ registry }.entities(input).component<ALL_COMPONENTS>(input);
        }
        catch (...) {
            LOG_ERROR << "Failed to load scene: \"" << filepath << "\"";
        }

        LOG_INFO << "Deserialise scene as binary: \"" << filepath << "\"";
    } else {
        if (filepath.empty())
            filepath = Engine::Get()->getApp()->getRootPath() / "assets/scenes" / (name + ".fsn");

        if (!fs::exists(filepath) || !fs::is_regular_file(filepath)) {
            LOG_ERROR << "No saved scene file found: \"" << filepath << "\"";
            return;
        }

        try {
            std::istringstream is{FileSystem::ReadText(filepath)};
            cereal::JSONInputArchive input{is};
            input(*this);

            entt::snapshot_loader{ registry }.entities(input).component<ALL_COMPONENTS>(input);
        }
        catch (...) {
            LOG_ERROR << "Failed to load scene: \"" << filepath << "\"";
        }

        LOG_INFO << "Deserialise scene: \"" << filepath << "\"";
    }

    systems.each([&](auto system) {
        system->setEnabled(true);
    });
}

void Scene::importMesh(const fs::path& filepath) {
    auto model = AssetRegistry::Get()->get_or_emplace<Model>(filepath);

    auto hierarchySystem = getSystem<HierarchySystem>();

    auto& root = model->getRoot();
    auto entity = createEntity(root->name);
    registry.emplace<TransformComponent>(entity);

    std::function<void(const std::shared_ptr<SceneObject>&)> createObject = [&](const std::shared_ptr<SceneObject>& object) {
        for (const auto& child : object->children) {
            auto childEntity = createEntity(child->name);
            registry.emplace<TransformComponent>(childEntity, child->position, child->oritentation, child->scale);

            if (child->meshes.size() == 1) {
                registry.emplace<MeshComponent>(childEntity, child->meshes[0]);
            } else {
                for (const auto& mesh: child->meshes) {
                    auto meshChildEntity = createEntity(child->name + " " + std::to_string(mesh->getMeshIndex()));
                    registry.emplace<TransformComponent>(meshChildEntity);
                    registry.emplace<MeshComponent>(meshChildEntity, mesh);
                    hierarchySystem->assignChild(childEntity, meshChildEntity);
                }
            }

            hierarchySystem->assignChild(entity, childEntity);

            createObject(child);
        }
    };

    createObject(root);
}