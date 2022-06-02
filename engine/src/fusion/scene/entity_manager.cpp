#include "entity_manager.hpp"

using namespace fe;
using namespace entt;

bool manager::has_children(const entity entity) const {
    if (auto component = try_get<RelationshipComponent>(entity); component && component->children > 0) {
        return true;
    }
    return false;
}

entity manager::get_parent(const entity entity) const {
    if (auto component = try_get<RelationshipComponent>(entity)) {
        return component->parent;
    }
    return entt::null;
}

void manager::destroy_parent(const entity parent) {
    for (auto child : get_children(parent)) {
        destroy(child);
    }
    destroy(parent);
}

void manager::remove_parent(const entity parent) {
    if (auto p = try_get<RelationshipComponent>(parent); p && p->children > 0) {
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
        remove<RelationshipComponent>(parent);
    }
}

void manager::assign_child(const entity parent, const entity child) {
    // Remove child from existing parent if any
    if (auto root = get_parent(child); root != entt::null) {
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

void manager::remove_child(const entity parent, const entity child) {
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

std::vector<entity> manager::get_children(const entity parent) const {
    std::vector<entity> ret;
    if (auto p = try_get<RelationshipComponent>(parent); p && p->children > 0) {
        ret.reserve(p->children);
        auto curr = p->first;
        while (curr != entt::null) {
            ret.push_back(curr);
            curr = get<RelationshipComponent>(curr).next;
        }
    }
    return ret;
}
