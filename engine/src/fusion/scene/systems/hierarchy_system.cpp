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
    auto nonHierarchyView = registry.view<TransformComponent>(entt::exclude<HierarchyComponent>);

    for (const auto& [entity, transform] : nonHierarchyView.each()) {
        transform.setWorldMatrix(glm::mat4{1.0f});
    }

    auto view = registry.view<TransformComponent, HierarchyComponent>();
    for (const auto& [entity, transform, hierarchy] : view.each()) {
        if (hierarchy.parent == entt::null) {
            // Recursively update children
            update(entity);
        }
    }

    /*auto transformGroup = registry.group<DirtyTransformComponent, TransformComponent>();
    transformGroup.sort([&](entt::entity lhs, entt::entity rhs) {
        auto clhs = registry.try_get<HierarchyComponent>(lhs);
        if (clhs == nullptr)
            return false;
        auto crhs = registry.try_get<HierarchyComponent>(rhs);
        if (crhs == nullptr)
            return false;
        return !(clhs->parent != entt::null && clhs->children < crhs->children);
    });

    for (auto [entity, transform] : transformGroup.each()) {
        transform.setWorldMatrix(getTRS(entity));
        LOG_INFO << "UPDATE";
    }

    registry.clear<DirtyTransformComponent>();*/
}

void HierarchySystem::onStop() {

}

void HierarchySystem::onEnabled() {
    //registry.on_construct<HierarchyComponent>().connect<&OnConstruct>();
    //registry.on_update<HierarchyComponent>().connect<&OnUpdate>();
    //registry.on_destroy<HierarchyComponent>().connect<&OnDestroy>();

    registry.on_construct<TransformComponent>().connect<&entt::registry::emplace<DirtyTransformComponent>>();
    registry.on_update<TransformComponent>().connect<&entt::registry::emplace_or_replace<DirtyTransformComponent>>();
}

void HierarchySystem::onDisabled() {
    //registry.on_construct<HierarchyComponent>().disconnect<&OnConstruct>();
    //registry.on_update<HierarchyComponent>().disconnect<&OnUpdate>();
    //registry.on_destroy<HierarchyComponent>().disconnect<&OnDestroy>();

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
            auto next = registry.get<HierarchyComponent>(child).next;
            update(child);
            child = next;
        }
    }
}

entt::entity HierarchySystem::getParent(entt::entity entity) const {
    if (auto hierarchy = registry.try_get<HierarchyComponent>(entity)) {
        return hierarchy->parent;
    }
    return entt::null;
}

bool HierarchySystem::isParent(entt::entity parent, entt::entity child) const {
    if (auto hierarchy = registry.try_get<HierarchyComponent>(child); hierarchy && hierarchy->parent == parent) {
        return true;
    }
    return false;
}

void HierarchySystem::destroyParent(entt::entity entity) {
    // Remove entity from existing parent if any
    if (auto parent = getParent(entity); parent != entt::null) {
        removeChild(parent, entity);
    }

    for (auto child : getChildren((entity))) {
        registry.destroy(child);
    }

    registry.destroy(entity);
}

void HierarchySystem::removeParent(entt::entity entity) {
    assert("FIX");
    if (auto hierarchy = registry.try_get<HierarchyComponent>(entity)) {
        auto child = hierarchy->first;
        while (child != entt::null) {
            auto& i = registry.get<HierarchyComponent>(child);
            auto next = i.next;
            if (i.children == 0) {
                registry.erase<HierarchyComponent>(child);
            } else {
                i.prev = entt::null;
                i.next = entt::null;
                i.parent = entt::null;
            }
            child = next;
        }

        auto parent = hierarchy->parent;
        if (parent != entt::null) {
            removeChild(parent, entity);
        } else {
            registry.erase<HierarchyComponent>(entity);
        }
    }
}

void HierarchySystem::assignChild(entt::entity parent, entt::entity child) {
    // Remove child from existing parent if any
    if (auto root = getParent(child); root != entt::null) {
        if (root == parent) return;
        removeChild(root, child);
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

void HierarchySystem::removeChild(entt::entity parent, entt::entity child) {
    if (auto hierarchy = registry.try_get<HierarchyComponent>(parent); hierarchy && hierarchy->children > 0) {
        if (hierarchy->children == 1) {
            registry.get<HierarchyComponent>(child).parent = entt::null;
            hierarchy->first = entt::null;
        } else {
            auto curr = hierarchy->first;
            while (curr != entt::null) {
                auto& i = registry.get<HierarchyComponent>(curr);
                if (curr == child) {
                    if (i.prev != entt::null) {
                        registry.get<HierarchyComponent>(i.prev).next = i.next;
                    } else {
                        // if prev null it should be first of parent
                        if (hierarchy->first == curr) {
                            hierarchy->first = i.next;
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
        hierarchy->children--;
        if (hierarchy->children == 0 && hierarchy->prev == entt::null && hierarchy->next == entt::null && hierarchy->parent == entt::null) {
            registry.erase<HierarchyComponent>(parent);
        }
    }
}

std::vector<entt::entity> HierarchySystem::getChildren(entt::entity entity) const {
    std::vector<entt::entity> children;
    if (auto hierarchy = registry.try_get<HierarchyComponent>(entity); hierarchy && hierarchy->children > 0) {
        children.reserve(hierarchy->children);
        auto child = hierarchy->first;
        while (child != entt::null) {
            children.push_back(child);
            child = registry.get<HierarchyComponent>(child).next;
        }
    }
    return children;
}

bool HierarchySystem::hasChildren(entt::entity entity) const {
    if (auto hierarchy = registry.try_get<HierarchyComponent>(entity); hierarchy && hierarchy->children > 0) {
        return true;
    }
    return false;
}