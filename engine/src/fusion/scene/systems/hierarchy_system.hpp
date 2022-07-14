#pragma once

#include "fusion/scene/system.hpp"
#include "fusion/scene/components.hpp"

namespace fe {
    class HierarchySystem final : public System {
    public:
        explicit HierarchySystem(entt::registry& registry);
        ~HierarchySystem() override;

        void onPlay() override;
        void onUpdate() override;
        void onStop() override;

        void reparent(entt::entity entity, entt::entity parent, HierarchyComponent& hierarchy);
        bool isParent(entt::entity entity, entt::entity child);
        void setParent(entt::entity entity, entt::entity parent);
        void destroyParent(entt::entity entity);
        std::vector<entt::entity> getChildren(entt::entity entity);

    private:
        void onEnabled() override;
        void onDisabled() override;

        static void OnConstruct(entt::registry& registry, entt::entity entity);
        static void OnDestroy(entt::registry& registry, entt::entity entity);
        static void OnUpdate(entt::registry& registry, entt::entity entity);
    };
}
