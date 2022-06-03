#pragma once

#include "world.hpp"

namespace fe {
    class EditorCamera;
    class Renderer;

    class Scene {
    public:
        Scene(std::string name);
        ~Scene();

        void onViewportResize(const glm::vec2& size);
        void onUpdateRuntime(float dt);
        void onUpdateEditor(float dt);

        void onRenderRuntime();
        void onRenderEditor(const EditorCamera& camera);

        std::string name;
        entt::world world;
    };
}
