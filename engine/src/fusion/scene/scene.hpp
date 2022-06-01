#pragma once

#include "entity_manager.hpp"

namespace fe {
    class EditorCamera;
    class Renderer;

    class Scene {
    public:
        Scene();
        ~Scene();

        void onViewportResize(const glm::vec2& size);
        void onUpdateRuntime(float dt);
        void onUpdateEditor(float dt);

        void onRenderRuntime();
        void onRenderEditor(const EditorCamera& camera);

        entt::manager manager;
    };
}
