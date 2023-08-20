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
#if FUSION_SCRIPTING
    addSystem<ScriptSystem>();
#endif
}

Scene::Scene(const Scene& other) : Scene{other.name} {
    registry.assign(other.registry.data(), other.registry.data() + other.registry.size(), other.registry.released());
    copyRegistry<ALL_COMPONENTS>(other.registry);
}

void Scene::onStart() {
    systems.each([](auto system) {
        system->setEnabled(true);
    });

    FE_LOG_DEBUG("Scene : '{}' created", name);
}

void Scene::onUpdate() {
    systems.each([](auto system) {
        if (system->isEnabled())
            system->onUpdate();
    });
}

void Scene::onPlay() {
    runtime = true;

    systems.each([](auto system) {
        if (system->isEnabled())
            system->onPlay();
    });

    FE_LOG_DEBUG("Scene : '{}' started runtime", name);
}

void Scene::onStop() {
    systems.each([](auto system) {
        if (system->isEnabled())
            system->onStop();
    });

    runtime = false;

    FE_LOG_DEBUG("Scene : '{}' stopped runtime", name);
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

entt::entity Scene::createEntity(std::string name) {
    auto entity = registry.create();

    if (name.empty())
        name = "Empty Entity";

    uint32_t i = 0;
    auto view = registry.view<NameComponent>();
    for (const auto& [e, n] : view.each()) {
        if (n.name.find(name) != std::string::npos) {
            ++i;
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

    if (auto scriptComponent = registry.try_get<ScriptComponent>(newEntity)) {
        ScriptEngine::Get()->onCreateEntity(newEntity, *scriptComponent);
    }

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

        FE_LOG_INFO("Serialise scene as binary: '{}'", filepath);
    } else {
        std::stringstream ss;
        {
            cereal::JSONOutputArchive output{ss};
            output(*this);
            entt::snapshot{ registry }.entities(output).component<ALL_COMPONENTS>(output);
        }

        FileSystem::WriteText(filepath, ss.str());

        FE_LOG_INFO("Serialise scene: '{}'", filepath);
    }
}

void Scene::deserialise(fs::path filepath, bool binary) {
    systems.each([&](auto system) {
        system->setEnabled(false);
    });

    if (filepath.empty())
        filepath = Engine::Get()->getApp()->getProjectSettings().projectRoot / "assets/scenes" / (name + ".fsn");

    if (binary) {
        if (!FileSystem::IsExists(filepath)) {
            FE_LOG_ERROR("No saved scene file found: '{}'", filepath);
            return;
        }

        try {
            std::ifstream is{filepath, std::ios::binary};
            cereal::BinaryInputArchive input{is};
            input(*this);

            entt::snapshot_loader{ registry }.entities(input).component<ALL_COMPONENTS>(input);
        }
        catch (std::exception& e) {
            FE_LOG_FATAL(e.what());
            FE_LOG_ERROR("Failed to load scene: '{}'", filepath);
        }

        FE_LOG_INFO("Deserialise scene as binary: '{}'", filepath);
    } else {
        if (!FileSystem::IsExists(filepath)) {
            FE_LOG_ERROR("No saved scene file found: '{}'", filepath);
            return;
        }

        try {
            std::istringstream is{FileSystem::ReadText(filepath)};
            cereal::JSONInputArchive input{is};
            input(*this);

            entt::snapshot_loader{ registry }.entities(input).component<ALL_COMPONENTS>(input);
        }
        catch (std::exception& e) {
            FE_LOG_FATAL(e.what());
            FE_LOG_ERROR("Failed to load scene: '{}'", filepath);
        }

        FE_LOG_INFO("Deserialise scene: '{}'", filepath);
    }

    systems.each([&](auto system) {
        system->setEnabled(true);
    });
}

void Scene::importMesh(fs::path filepath) {
    filepath += ".meta";
    auto uuid = uuids::uuid::from_string(FileSystem::ReadText(filepath));
    if (!uuid.has_value()) {
        FE_LOG_ERROR("Cannot find asset. Wrong metadata: '{}'", filepath);
        return;
    }

    auto model = AssetRegistry::Get()->load<Model>(*uuid);
    if (model == nullptr || !model->isLoaded()) {
        FE_LOG_ERROR("Cannot load model");
        return;
    }

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