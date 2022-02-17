#pragma once

#include <entt/entity/registry.hpp>

namespace Fusion {
    class EditorCamera;
    class MeshRenderer;

    class FUSION_API Scene {
    public:
        Scene();
        ~Scene();
        FE_NONCOPYABLE(Scene);

        void onUpdateRuntime();
        void onUpdateEditor();

        void onRenderRuntime(MeshRenderer& meshRenderer);
        void onRenderEditor(MeshRenderer& meshRenderer);

        entt::entity getPrimaryCameraEntity() const { return entt::null; }
        entt::registry& getEntityRegistry() { return registry; }

    private:
        entt::registry registry;

        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
    };
}
