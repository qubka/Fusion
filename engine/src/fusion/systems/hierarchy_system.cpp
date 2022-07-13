#include "hierarchy_system.hpp"
#include "fusion/scene/components.hpp"

using namespace fe;

bool ParentSystem::has_children(const entt::entity entity) const {
    if (auto component = registry.try_get<HierarchyComponent>(entity); component && component->children > 0) {
        return true;
    }
    return false;
}

entt::entity ParentSystem::get_parent(const entt::entity entity) const {
    if (auto component = registry.try_get<HierarchyComponent>(entity)) {
        return component->parent;
    }
    return entt::null;
}

void ParentSystem::destroy_parent(const entt::entity entity) {
    // Remove entity from existing parent if any
    if (auto parent = get_parent(entity); parent != entt::null) {
        remove_child(parent, entity);
    }
    each_child(entity, [&](const entt::entity e) {
        registry.destroy(e);
    });
    registry.destroy(entity);
}

void ParentSystem::remove_parent(const entt::entity entity) {
    if (auto component = registry.try_get<HierarchyComponent>(entity); component && component->children > 0) {
        auto curr = component->first;
        while (curr != entt::null) {
            auto& i = registry.get<HierarchyComponent>(curr);
            auto next = i.next;
            if (i.children == 0) {
                registry.erase<HierarchyComponent>(curr);
            } else {
                i.prev = entt::null;
                i.next = entt::null;
                i.parent = entt::null;
            }
            curr = next;
        }
        registry.erase<HierarchyComponent>(entity);
    }
}

void ParentSystem::assign_child(const entt::entity parent, const entt::entity child) {
    // Remove child from existing parent if any
    if (auto root = get_parent(child); root != entt::null) {
        if (root == parent) return;
        remove_child(root, child);
    }

    auto& p = registry.get_or_emplace<HierarchyComponent>(parent);
    if (p.children == 0) {
        p.first = child;

        auto& c = registry.get_or_emplace<HierarchyComponent>(child);
        c.prev = entt::null;
        c.next = entt::null;
        c.parent = parent;
    } else {
        auto curr = p.first;
        while (curr != entt::null) {
            auto& i = registry.get<HierarchyComponent>(curr);
            if (i.next == entt::null) {
                i.next = child;

                auto& c = registry.get_or_emplace<HierarchyComponent>(child);
                c.prev = curr;
                c.next = entt::null;
                c.parent = parent;
                break;
            } else {
                curr = i.next;
            }
        }
    }

    p.children++;
}

void ParentSystem::remove_child(const entt::entity parent, const entt::entity child) {
    if (auto component = registry.try_get<HierarchyComponent>(parent); component && component->children > 0) {
        if (component->children == 1) {
            registry.get<HierarchyComponent>(child).parent = entt::null;
            component->first = entt::null;
        } else {
            auto curr = component->first;
            while (curr != entt::null) {
                auto& i = registry.get<HierarchyComponent>(curr);
                if (curr == child) {
                    if (i.prev != entt::null) {
                        registry.get<HierarchyComponent>(i.prev).next = i.next;
                    } else {
                        // if prev null it should be first of parent
                        if (component->first == curr) {
                            component->first = i.next;
                        }
                    }
                    if (i.next != entt::null) {
                        registry.get<HierarchyComponent>(i.next).prev = i.prev;
                    }
                    if (i.children == 0) {
                        registry.erase<HierarchyComponent>(curr);
                    } else {
                        i.prev = entt::null;
                        i.next = entt::null;
                        i.parent = entt::null;
                    }
                    break;
                }
                curr = i.next;
            }
        }
        component->children--;
        if (component->children == 0 && component->prev == entt::null && component->next == entt::null && component->parent == entt::null) {
            registry.erase<HierarchyComponent>(parent);
        }
    }
}

void ParentSystem::each_child(const entt::entity entity, const std::function<void(const entt::entity)>& function) const {
    if (auto component = registry.try_get<HierarchyComponent>(entity); component && component->children > 0) {
        auto curr = component->first;
        while (curr != entt::null) {
            function(curr);
            curr = registry.get<HierarchyComponent>(curr).next;
        }
    }
}
