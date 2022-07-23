#include "scene.hpp"
#include "system.hpp"
#include "components.hpp"

#include "fusion/utils/string.hpp"
#include "fusion/filesystem/file_system.hpp"
#include "fusion/filesystem/virtual_file_system.hpp"

#include "fusion/scene/systems/hierarchy_system.hpp"

#include <entt/entt.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

using namespace fe;

Scene::Scene(std::string name) : name{std::move(name)} {
    addSystem<HierarchySystem>();
}

void Scene::onStart() {
    systems.each([&](auto system) {
        system->setEnabled(true);
    });

    LOG_DEBUG << "Scene : \"" << name << "\" created first time";
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

entt::entity Scene::createEntity(std::string name) {
    auto entity = registry.create();
    registry.emplace<IdComponent>(entity);
    //registry.emplace<TransformComponent>(entity);

    if (name.empty())
        name = "Empty Entity";

    uint32_t i = 0;
    auto view = registry.view<NameComponent>();
    for (auto [e, str] : view.each()) {
        if (String::Contains(str, name)) {
            i++;
        }
    }
    if (i > 0)
        name += " (" + std::to_string(i) + ")";

    registry.emplace<NameComponent>(entity, name);

    return entity;
}

void Scene::destroyEntity(entt::entity entity) {
    // TODO: Children ?
    registry.destroy(entity);
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

    for (auto child : children)  {
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
            filepath = VirtualFileSystem::Get()->resolvePhysicalPath("Scenes"_p / (name + ".bin"));

        std::ofstream os{filepath, std::ios::binary};
        {
            // output finishes flushing its contents when it goes out of scope
            cereal::BinaryOutputArchive output{os};
            output(*this);
            entt::snapshot{ registry }.entities(output).component<ALL_COMPONENTS>(output);
        }

        LOG_INFO << "Serialise scene as binary: \"" << filepath << "\"";
    } else {
        if (filepath.empty())
            filepath = VirtualFileSystem::Get()->resolvePhysicalPath("Scenes"_p / (name + ".fsn"));

        std::stringstream ss;
        {
            // output finishes flushing its contents when it goes out of scope
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
            filepath = VirtualFileSystem::Get()->resolvePhysicalPath("Scenes"_p / (name + ".bin"));

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
            filepath = VirtualFileSystem::Get()->resolvePhysicalPath("Scenes"_p / (name + ".fsn"));

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