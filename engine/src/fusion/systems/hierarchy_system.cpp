#include "hierarchy_system.hpp"
#include "fusion/scene/components.hpp"

using namespace fe;

bool HierarchySystem::hasChildren(entt::entity entity) const {
    if (auto component = registry.try_get<HierarchyComponent>(entity); component && component->first != entt::null) {
        return true;
    }
    return false;
}

entt::entity HierarchySystem::getParent(entt::entity entity) const {
    if (auto component = registry.try_get<HierarchyComponent>(entity)) {
        return component->parent;
    }
    return entt::null;
}

bool HierarchySystem::isParent(entt::entity parent, entt::entity child) const {
    if (auto component = registry.try_get<HierarchyComponent>(child)) {
        return component->parent == parent;
    }
    return false;
}

void HierarchySystem::destroyParent(entt::entity entity) {
    // Remove entity from existing parent if any
    if (auto parent = getParent(entity); parent != entt::null) {
        removeChild(parent, entity);
    }
    for (auto& child : getChildren(entity)) {
        registry.destroy(child);
    }
    registry.destroy(entity);
}

void HierarchySystem::removeParent(entt::entity entity) {
    if (auto component = registry.try_get<HierarchyComponent>(entity); component && component->first != entt::null) {
        auto curr = component->first;
        while (curr != entt::null) {
            auto& i = registry.get<HierarchyComponent>(curr);
            auto next = i.next;
            if (i.first == entt::null) {
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

void HierarchySystem::assignChild(entt::entity parent, entt::entity child) {
    // Remove child from existing parent if any
    if (auto root = getParent(child); root != entt::null) {
        if (root == parent) return;
        removeChild(root, child);
    }

    auto& p = registry.get_or_emplace<HierarchyComponent>(parent);
    if (p.first == entt::null) {
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

void HierarchySystem::removeChild(entt::entity parent, entt::entity child) {
    if (auto component = registry.try_get<HierarchyComponent>(parent); component && component->first != entt::null) {
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
                    if (i.first == entt::null) {
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
        if (component->first == entt::null && component->prev == entt::null && component->next == entt::null && component->parent == entt::null) {
            registry.erase<HierarchyComponent>(parent);
        }
    }
}

std::vector<entt::entity> HierarchySystem::getChildren(entt::entity entity) const {
    std::vector<entt::entity> children;
    if (auto component = registry.try_get<HierarchyComponent>(entity); component && component->first != entt::null) {
        children.reserve(component->children);
        auto curr = component->first;
        while (curr != entt::null) {
            children.push_back(curr);
            curr = registry.get<HierarchyComponent>(curr).next;
        }
    }
    return children;
}
