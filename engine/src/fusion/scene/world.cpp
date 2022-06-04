#include "world.hpp"

using namespace fe;
using namespace entt;

glm::mat4 world::transform(const entity entity) const {
    /*glm::mat4 localTransform{ get<TransformComponent>(entity).transform() };
    if (auto component = try_get<RelationshipComponent>(entity); component && component->parent != entt::null) {
        return localTransform * transform(component->parent);
    } else {
        return localTransform;
    }*/
    return glm::mat4{ 1.0f };
}

bool world::has_children(const entity entity) const {
    if (auto component = try_get<RelationshipComponent>(entity); component && component->children > 0) {
        return true;
    }
    return false;
}

entity world::get_parent(const entity entity) const {
    if (auto component = try_get<RelationshipComponent>(entity)) {
        return component->parent;
    }
    return entt::null;
}

void world::destroy_parent(const entity entity) {
    for (auto e : get_children(entity)) {
        destroy(e);
    }
    destroy(entity);
}

void world::remove_parent(const entity entity) {
    if (auto p = try_get<RelationshipComponent>(entity); p && p->children > 0) {
        auto curr = p->first;
        while (curr != entt::null) {
            auto& i = get<RelationshipComponent>(curr);
            auto next = i.next;
            if (i.children == 0) {
                remove<RelationshipComponent>(curr);
            } else {
                i.prev = entt::null;
                i.next = entt::null;
                i.parent = entt::null;
            }
            curr = next;
        }
        remove<RelationshipComponent>(entity);
    }
}

void world::assign_child(const entity parent, const entity child) {
    // Remove child from existing parent if any
    if (auto root = get_parent(child); root != entt::null) {
        if (root == parent) return;
        remove_child(root, child);
    }

    auto& p = get_or_emplace<RelationshipComponent>(parent);
    if (p.children == 0) {
        p.first = child;

        auto& c = get_or_emplace<RelationshipComponent>(child);
        c.prev = entt::null;
        c.next = entt::null;
        c.parent = parent;
    } else {
        auto curr = p.first;
        while (curr != entt::null) {
            auto& i = get<RelationshipComponent>(curr);
            if (i.next == entt::null) {
                i.next = child;

                auto& c = get_or_emplace<RelationshipComponent>(child);
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

void world::remove_child(const entity parent, const entity child) {
    if (auto p = try_get<RelationshipComponent>(parent); p && p->children > 0) {
        if (p->children == 1) {
            get<RelationshipComponent>(child).parent = entt::null;
            p->first = entt::null;
        } else {
            auto curr = p->first;
            while (curr != entt::null) {
                auto& i = get<RelationshipComponent>(curr);
                if (curr == child) {
                    if (i.prev != entt::null) {
                        get<RelationshipComponent>(i.prev).next = i.next;
                    } else {
                        // if prev null it should be first of parent
                        if (p->first == curr) {
                            p->first = i.next;
                        }
                    }
                    if (i.next != entt::null) {
                        get<RelationshipComponent>(i.next).prev = i.prev;
                    }
                    if (i.children == 0) {
                        remove<RelationshipComponent>(curr);
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
        p->children--;
        if (p->children == 0 && p->prev == entt::null && p->next == entt::null && p->parent == entt::null) {
            remove<RelationshipComponent>(parent);
        }
    }
}

std::vector<entity> world::get_children(const entity entity) const {
    std::vector<entt::entity> ret;
    if (auto p = try_get<RelationshipComponent>(entity); p && p->children > 0) {
        ret.reserve(p->children);
        auto curr = p->first;
        while (curr != entt::null) {
            ret.push_back(curr);
            curr = get<RelationshipComponent>(curr).next;
        }
    }
    return ret;
}

void world::notify_children(const entity entity) {
    get_or_emplace<DirtyComponent>(entity);
    if (auto p = try_get<RelationshipComponent>(entity); p && p->children > 0) {
        auto curr = p->first;
        while (curr != entt::null) {
            notify_children(curr);
            curr = get<RelationshipComponent>(curr).next;
        }
    }
}

glm::mat4 world::make_local_to_world(const entity entity) const {
    if (auto component = try_get<RelationshipComponent>(entity); component && component->parent != entt::null) {
        return make_local_to_world(component->parent) * get<TransformComponent>(entity).make_local_to_parent();
    } else {
        return get<TransformComponent>(entity).make_local_to_parent();
    }
}

glm::mat4 world::make_world_to_local(const entity entity) const {
    if (auto component = try_get<RelationshipComponent>(entity); component && component->parent != entt::null) {
        return get<TransformComponent>(entity).make_parent_to_local() * make_world_to_local(component->parent);
    } else {
        return get<TransformComponent>(entity).make_parent_to_local();
    }
}