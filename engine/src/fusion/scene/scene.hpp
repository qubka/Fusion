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
        void onUpdateRuntime(float ts);
        void onUpdateEditor(float ts);

        void onRenderRuntime(Renderer& renderer);
        void onRenderEditor(Renderer& renderer);

        entt::registry registry;
    };
}
