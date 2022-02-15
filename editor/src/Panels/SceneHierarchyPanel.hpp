#pragma once

#include "Fusion/Scene/Scene.hpp"

namespace Fusion {
    class Mesh;
    class FUSION_API SceneHierarchyPanel {
    public:
        SceneHierarchyPanel();
        ~SceneHierarchyPanel() = default;

        void setContext(const std::shared_ptr<Scene>& scene);

        void onImGui();

        entt::entity getSelectedEntity() const { return selectionContext; }

    private:
        void drawEntity(entt::entity entity);
        void drawComponents(entt::entity entity);

        void drawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
        template<typename T>
        void drawComponent(const std::string& name, entt::entity entity, std::function<void(T& comp)>&& function);

    private:
        std::shared_ptr<Scene> context;
        std::shared_ptr<Mesh> mesh;
        entt::entity selectionContext;
    };

}