#pragma once

#include "fusion/scene/scene.hpp"

namespace fe {
    class ContentBrowserPanel;
    class SceneHierarchyPanel {
    public:
        SceneHierarchyPanel(ContentBrowserPanel& contentBrowserPanel) : contentBrowserPanel{contentBrowserPanel} {};
        ~SceneHierarchyPanel() = default;

        void setContext(const std::shared_ptr<Scene>& scene);

        void onImGui();

        entt::entity getSelectedEntity() const { return selectionContext; }

    private:
        void drawEntity(entt::entity entity);
        void drawComponents(entt::entity entity);
        template<typename T>
        void drawComponent(const std::string& name, entt::entity entity, std::function<void(T& comp)>&& function, bool removable = true);
        void drawFileBrowser(const std::string& label, std::string& value, const std::string& file, const std::vector<std::string>& formats, float columnWidth = 100.0f);
        static void drawVec2Control(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f);
        static void drawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

    private:
        std::shared_ptr<Scene> context;
        entt::entity selectionContext;
        entt::entity renameContext;
        ContentBrowserPanel& contentBrowserPanel;
        std::string filter;
    };
}
