#pragma once

#include <entt/entity/registry.hpp>

namespace Fusion {
    class EditorCamera;

    class Scene {
    public:
        Scene();
        ~Scene();
        FE_NONCOPYABLE(Scene);

        void onUpdateRuntime();
        void onUpdateEditor();

        void onRenderRuntime();
        void onRenderEditor();

        entt::entity getPrimaryCameraEntity() const { return entt::null; }
        entt::registry& getEntityRegistry() { return registry; }

    private:
        entt::registry registry;

        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
    };
}