#pragma once

namespace fe {
    template<typename T>
    void onComponentConstruct(entt::registry& registry, entt::entity entity);
    template<typename T>
    void onComponentUpdate(entt::registry& registry, entt::entity entity);
    template<typename T>
    void onComponentDestroy(entt::registry& registry, entt::entity entity);
}
