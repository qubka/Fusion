#include "hierarchy_system.hpp"

using namespace fe;

HierarchySystem::HierarchySystem(entt::registry& registry) : System{registry} {
}

HierarchySystem::~HierarchySystem() {
    if (enabled) {
        onDisabled();
    }
}

void HierarchySystem::onPlay() {

}

void HierarchySystem::onUpdate() {
    /*auto nonHierarchyView = registry.view<TransformComponent>(entt::exclude<HierarchyComponent>);

    for (const auto& [entity, transform] : nonHierarchyView.each()) {
        transform.setWorldMatrix(glm::mat4{1.0f});
    }

    auto view = registry.view<TransformComponent, HierarchyComponent>();
    for (const auto& [entity, transform, hierarchy] : view.each()) {
        if (hierarchy.parent == entt::null) {
            // Recursively update children
            update(entity);
        }
    }*/

    auto transformGroup = registry.group<DirtyTransformComponent, TransformComponent>();
    transformGroup.sort([&](const entt::entity lhs, const entt::entity rhs) {
        auto clhs = registry.try_get<HierarchyComponent>(lhs);
        if (clhs == nullptr)
            return false;
        auto crhs = registry.try_get<HierarchyComponent>(rhs);
        if (crhs == nullptr)
            return false;
        //return !(clhs->parent != entt::null && clhs->children < crhs->children);
        return !(clhs->parent != entt::null && getChildren(lhs).size() < getChildren(rhs).size());
    });

    for (auto [entity, transform] : transformGroup.each()) {
        transform.setWorldMatrix(glm::mat4{1.0f});
        update(entity);
    }

    registry.clear<DirtyTransformComponent>();
}

void HierarchySystem::onStop() {

}

void HierarchySystem::onEnabled() {
    registry.on_construct<HierarchyComponent>().connect<&OnConstruct>();
    registry.on_update<HierarchyComponent>().connect<&OnUpdate>();
    registry.on_destroy<HierarchyComponent>().connect<&OnDestroy>();

    registry.on_construct<TransformComponent>().connect<&entt::registry::emplace<DirtyTransformComponent>>();
    registry.on_update<TransformComponent>().connect<&entt::registry::emplace_or_replace<DirtyTransformComponent>>();
}

void HierarchySystem::onDisabled() {
    registry.on_construct<HierarchyComponent>().disconnect<&OnConstruct>();
    registry.on_update<HierarchyComponent>().disconnect<&OnUpdate>();
    registry.on_destroy<HierarchyComponent>().disconnect<&OnDestroy>();

    registry.on_construct<TransformComponent>().disconnect<&entt::registry::emplace<DirtyTransformComponent>>();
    registry.on_update<TransformComponent>().disconnect<&entt::registry::emplace_or_replace<DirtyTransformComponent>>();
}

void HierarchySystem::update(entt::entity entity) {
    if (auto hierarchy = registry.try_get<HierarchyComponent>(entity)) {
        if (auto transform = registry.try_get<TransformComponent>(entity)) {
            if (hierarchy->parent != entt::null) {
                auto parentTransform = registry.try_get<TransformComponent>(hierarchy->parent);
                if (parentTransform) {
                    transform->setWorldMatrix(parentTransform->getWorldMatrix());
                }
            } else {
                transform->setWorldMatrix(glm::mat4{1.0f});
            }
        }

        entt::entity child = hierarchy->first;
        while (child != entt::null) {
            hierarchy = registry.try_get<HierarchyComponent>(child);
            auto next = hierarchy ? hierarchy->next : entt::null;
            update(child);
            child = next;
        }
    }
}

void HierarchySystem::reparent(entt::entity entity, entt::entity parent, HierarchyComponent& hierarchy) {
    HierarchySystem::OnDestroy(registry, entity);

    hierarchy.parent = entt::null;
    hierarchy.next = entt::null;
    hierarchy.prev = entt::null;

    if (parent != entt::null) {
        hierarchy.parent = parent;
        HierarchySystem::OnConstruct(registry, entity);
    }
}

bool HierarchySystem::isParent(entt::entity entity, entt::entity child) {
    if (auto hierarchy = registry.try_get<HierarchyComponent>(child)) {
        auto parent = hierarchy->parent;
        while (parent != entt::null) {
            if (parent == entity) {
                return true;
            } else {
                hierarchy = registry.try_get<HierarchyComponent>(parent);
                parent = hierarchy ? hierarchy->parent : entt::null;
            }
        }
    }

    return false;
}

void HierarchySystem::setParent(entt::entity entity, entt::entity parent) {
    bool acceptable;
    auto hierarchy = registry.try_get<HierarchyComponent>(entity);
    if (hierarchy != nullptr) {
        acceptable = parent != entity && (!isParent(parent, entity)) && (hierarchy->parent != entity);
    } else
        acceptable = parent != entity;

    if (!acceptable)
        return;

    if (hierarchy)
        reparent(entity, parent, *hierarchy);
    else {
        registry.emplace<HierarchyComponent>(entity, parent);
    }
}

void HierarchySystem::destroyParent(entt::entity entity) {
    if (auto hierarchy = registry.try_get<HierarchyComponent>(entity)) {
        auto child = hierarchy->first;
        while (child != entt::null) {
            hierarchy = registry.try_get<HierarchyComponent>(child);
            auto next = hierarchy ? hierarchy->next : entt::null;
            destroyParent(child);
            child = next;
        }
    }
    registry.destroy(entity);
}

std::vector<entt::entity> HierarchySystem::getChildren(entt::entity entity) {
    std::vector<entt::entity> children;

    if (auto hierarchy = registry.try_get<HierarchyComponent>(entity)) {
        auto child = hierarchy->first;
        while (child != entt::null && registry.valid(child)) {
            children.push_back(child);
            hierarchy = registry.try_get<HierarchyComponent>(child);
            child = hierarchy ? hierarchy->next : entt::null;
        }
    }

    return children;
}

void HierarchySystem::OnConstruct(entt::registry& registry, entt::entity entity) {
    auto& hierarchy = registry.get<HierarchyComponent>(entity);
    if (hierarchy.parent != entt::null) {
        auto& parentHierarchy = registry.get_or_emplace<HierarchyComponent>(hierarchy.parent);
        if (parentHierarchy.first == entt::null) {
            parentHierarchy.first = entity;
        } else {
            // get last children
            auto prev = parentHierarchy.first;
            auto currentHierarchy = registry.try_get<HierarchyComponent>(prev);
            while (currentHierarchy != nullptr && currentHierarchy->next != entt::null) {
                prev = currentHierarchy->next;
                currentHierarchy = registry.try_get<HierarchyComponent>(prev);
            }
            // add new
            currentHierarchy->next = entity;
            hierarchy.prev = prev;
        }
    }
}

void HierarchySystem::OnDestroy(entt::registry& registry, entt::entity entity) {
    auto& hierarchy = registry.get<HierarchyComponent>(entity);
    // if is the first child
    if (hierarchy.prev == entt::null || !registry.valid(hierarchy.prev)) {
        if (hierarchy.parent != entt::null && registry.valid(hierarchy.parent)) {
            if (auto parentHierarchy = registry.try_get<HierarchyComponent>(hierarchy.parent)) {
                parentHierarchy->first = hierarchy.next;
                if (hierarchy.next != entt::null) {
                    if (auto nextHierarchy = registry.try_get<HierarchyComponent>(hierarchy.next)) {
                        nextHierarchy->prev = entt::null;
                    }
                }
            }
        }
    } else {
        if (auto prevHierarchy = registry.try_get<HierarchyComponent>(hierarchy.prev)) {
            prevHierarchy->next = hierarchy.next;
        }
        if (hierarchy.next != entt::null) {
            if (auto nextHierarchy = registry.try_get<HierarchyComponent>(hierarchy.next)) {
                nextHierarchy->prev = hierarchy.prev;
            }
        }
    }
}

void HierarchySystem::OnUpdate(entt::registry& registry, entt::entity entity) {
    auto& hierarchy = registry.get<HierarchyComponent>(entity);
    // if is the first child
    if (hierarchy.prev == entt::null) {
        if (hierarchy.parent != entt::null) {
            if (auto parentHierarchy = registry.try_get<HierarchyComponent>(hierarchy.parent)) {
                parentHierarchy->first = hierarchy.next;
                if (hierarchy.next != entt::null) {
                    if (auto nextHierarchy = registry.try_get<HierarchyComponent>(hierarchy.next)) {
                        nextHierarchy->prev = entt::null;
                    }
                }
            }
        }
    } else {
        if (auto prevHierarchy = registry.try_get<HierarchyComponent>(hierarchy.prev)) {
            prevHierarchy->next = hierarchy.next;
        }
        if (hierarchy.next != entt::null) {
            if (auto nextHierarchy = registry.try_get<HierarchyComponent>(hierarchy.next)) {
                nextHierarchy->prev = hierarchy.prev;
            }
        }
    }
}
