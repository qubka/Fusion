#pragma once

#include <entt/entity/registry.hpp>

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

        void onRenderRuntime(Renderer& renderer);
        void onRenderEditor(Renderer& renderer);

        entt::registry registry;
    };
}
