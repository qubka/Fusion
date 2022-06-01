#include "entity.hpp"

/*void fe::set_parent(entt::registry& registry, entt::entity parent, entt::entity child) {
    auto& p = registry.get_or_emplace<RelationshipComponent>(parent);
    p.children++;
    if (p.children == 1) {
        p.first = child;
    } if (p.children == 2) {
        p.next = child;
    }

    auto& c = registry.get_or_emplace<RelationshipComponent>(child);
    c.parent = parent;
    entt::to_integer
}
*/