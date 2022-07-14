#include "hierarchy_system.hpp"

using namespace fe;

HierarchySystem::HierarchySystem(entt::registry& registry) : System{registry} {
}

HierarchySystem::~HierarchySystem() {
    if (enabled) {
        onDisabled();
    }
}

void HierarchySystem::onStart() {
    if (!enabled) {
        onEnabled();
        enabled = true;
    }
}

void HierarchySystem::onPlay() {

}

void HierarchySystem::onUpdate() {

}

void HierarchySystem::onStop() {

}

void HierarchySystem::onEnabled() {
    registry.on_construct<HierarchyComponent>().connect<&OnConstruct>();
    registry.on_update<HierarchyComponent>().connect<&OnUpdate>();
    registry.on_destroy<HierarchyComponent>().connect<&OnDestroy>();
}

void HierarchySystem::onDisabled() {
    registry.on_construct<HierarchyComponent>().disconnect<&OnConstruct>();
    registry.on_update<HierarchyComponent>().disconnect<&OnUpdate>();
    registry.on_destroy<HierarchyComponent>().disconnect<&OnDestroy>();
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
    bool acceptable = false;
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
            auto childHierarchy = registry.try_get<HierarchyComponent>(child);
            auto next = childHierarchy ? childHierarchy->next : entt::null;
            destroyParent(child);
            child = next;
        }
    }
    registry.destroy(entity);
}

void HierarchySystem::OnConstruct(entt::registry& registry, entt::entity entity) {
    auto& hierarchy = registry.get<HierarchyComponent>(entity);
    if (hierarchy.parent != entt::null) {
        auto& parent_hierarchy = registry.get_or_emplace<HierarchyComponent>(hierarchy.parent);
        if (parent_hierarchy.first == entt::null) {
            parent_hierarchy.first = entity;
        } else {
            // get last children
            auto prev_ent = parent_hierarchy.first;
            auto current_hierarchy = registry.try_get<HierarchyComponent>(prev_ent);
            while (current_hierarchy != nullptr && current_hierarchy->next != entt::null) {
                prev_ent = current_hierarchy->next;
                current_hierarchy = registry.try_get<HierarchyComponent>(prev_ent);
            }
            // add new
            current_hierarchy->next = entity;
            hierarchy.prev = prev_ent;
        }
    }
}

void HierarchySystem::OnDestroy(entt::registry& registry, entt::entity entity) {
    auto& hierarchy = registry.get<HierarchyComponent>(entity);
    // if is the first child
    if (hierarchy.prev == entt::null || !registry.valid(hierarchy.prev)) {
        if (hierarchy.parent != entt::null && registry.valid(hierarchy.parent)) {
            if (auto parent_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.parent)) {
                parent_hierarchy->first = hierarchy.next;
                if (hierarchy.next != entt::null) {
                    auto next_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.next);
                    if (next_hierarchy != nullptr) {
                        next_hierarchy->prev = entt::null;
                    }
                }
            }
        }
    } else {
        if (auto prev_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.prev)) {
            prev_hierarchy->next = hierarchy.next;
        }
        if (hierarchy.next != entt::null) {
            if (auto next_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.next)) {
                next_hierarchy->prev = hierarchy.prev;
            }
        }
    }
}

void HierarchySystem::OnUpdate(entt::registry& registry, entt::entity entity) {
    auto& hierarchy = registry.get<HierarchyComponent>(entity);
    // if is the first child
    if (hierarchy.prev == entt::null) {
        if (hierarchy.parent != entt::null) {
            if (auto parent_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.parent)) {
                parent_hierarchy->first = hierarchy.next;
                if (hierarchy.next != entt::null) {
                    if (auto next_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.next)) {
                        next_hierarchy->prev = entt::null;
                    }
                }
            }
        }
    } else {
        if (auto prev_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.prev)) {
            prev_hierarchy->next = hierarchy.next;
        }
        if (hierarchy.next != entt::null) {
            if (auto next_hierarchy = registry.try_get<HierarchyComponent>(hierarchy.next)) {
                next_hierarchy->prev = hierarchy.prev;
            }
        }
    }
}