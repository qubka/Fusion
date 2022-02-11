#pragma once

#include <entt/entity/registry.hpp>

namespace Fusion {
    class EditorCamera;

    class FUSION_API Scene {
    public:
        Scene();
        ~Scene();

        void onUpdateRuntime();
        void onUpdateEditor(EditorCamera& camera);

        entt::entity getPrimaryCameraEntity();

    private:
        entt::registry registry;

        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
    };
}
