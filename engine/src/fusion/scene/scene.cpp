#include "scene.h"
#include "system.h"
#include "components.h"

#include "fusion/core/engine.h"
#include "fusion/models/model.h"
#include "fusion/filesystem/file_system.h"
#include "fusion/scene/systems/hierarchy_system.h"
#include "fusion/scene/systems/camera_system.h"
#include "fusion/scene/systems/physics_system.h"
#include "fusion/scene/systems/script_system.h"

using namespace fe;

Scene::Scene(std::string_view name) : name{name} {
    addSystem<HierarchySystem>();
    addSystem<CameraSystem>();
    addSystem<PhysicsSystem>();
    addSystem<ScriptSystem>();
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

    if (name.empty())
        name = "Empty Entity";

    uint32_t i = 0;
    auto view = registry.view<NameComponent>();
    for (const auto& [e, n] : view.each()) {
        if (n.name.find(name) != std::string::npos) {
            i++;
        }
    }
    if (i > 0)
        name += " (" + std::to_string(i) + ")";

    registry.emplace<NameComponent>(entity, std::move(name));

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

entt::entity Scene::getEntityByName(std::string_view name) {
    auto view = registry.view<NameComponent>();
    for (auto [e, n] : view.each()) {
        if (n.name == name)
            return e;
    }
    return entt::null;
}

bool Scene::isEntityValid(entt::entity entity) {
    return entity != entt::null && registry.valid(entity);
}

void Scene::serialise(fs::path filepath, bool binary) {
    if (filepath.empty())
        filepath = Engine::Get()->getApp()->getProjectSettings().projectRoot / "assets/scenes" / (name + ".fsn");

    if (binary) {
        std::ofstream os{filepath, std::ios::binary};
        {
            cereal::BinaryOutputArchive output{os};
            output(*this);
            entt::snapshot{ registry }.entities(output).component<ALL_COMPONENTS>(output);
        }

        LOG_INFO << "Serialise scene as binary: \"" << filepath << "\"";
    } else {
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

    if (filepath.empty())
        filepath = Engine::Get()->getApp()->getProjectSettings().projectRoot / "assets/scenes" / (name + ".fsn");

    if (binary) {
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
    auto model = AssetRegistry::Get()->load<Model>(filepath);

    auto hierarchySystem = getSystem<HierarchySystem>();

    auto& root = model->getRoot();
    auto entity = createEntity(root.name);
    registry.emplace<TransformComponent>(entity, root.position, root.orientation, root.scale);
    if (root.children.size() == 1) {
        auto& main = root.children.front();
        if (main.meshes.size() == 1 && main.children.empty())
            registry.emplace<MeshComponent>(entity, model, main.meshes.front()->getIndex());
    } else {
        std::function<void(const SceneObject&)> createObject = [&](const SceneObject& object) {
            for (const auto& child : object.children) {
                auto childEntity = createEntity(child.name);
                registry.emplace<TransformComponent>(childEntity, child.position, child.orientation, child.scale);

                if (child.meshes.size() == 1) {
                    registry.emplace<MeshComponent>(childEntity, model, child.meshes.front()->getIndex());
                } else {
                    for (const auto& mesh: child.meshes) {
                        auto meshChildEntity = createEntity(child.name + " " + std::to_string(mesh->getIndex()));
                        registry.emplace<TransformComponent>(meshChildEntity);
                        registry.emplace<MeshComponent>(meshChildEntity, model, mesh->getIndex());
                        hierarchySystem->assignChild(childEntity, meshChildEntity);
                    }
                }

                hierarchySystem->assignChild(entity, childEntity);

                createObject(child);
            }
        };

        createObject(root);
    }
}