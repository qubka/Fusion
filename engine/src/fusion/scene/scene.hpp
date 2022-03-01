#pragma once

#include <entt/entity/registry.hpp>

namespace fe {
    class EditorCamera;

    class Scene {
    public:
        Scene();
        ~Scene();

        void onUpdateRuntime();
        void onUpdateEditor();

        void onRenderRuntime();
        void onRenderEditor();

        entt::registry registry;
    };
}
