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
        void drawEntities();
        void drawComponents(entt::entity entity);
        template<typename T>
        void drawComponent(const std::string& label, entt::entity entity, std::function<void(T& comp)>&& function);
        bool drawFileBrowser(const std::string& label, std::string& value, const std::vector<std::string>& formats, float columnWidth = 120.0f);
        static bool drawVec2Control(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 120.0f);
        static bool drawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 120.0f);
        static bool drawValueControl(const std::string& label, std::function<bool()>&& function, float columnWidth = 120.0f);

    private:
        std::shared_ptr<Scene> context;
        entt::entity selectionContext;
        entt::entity renameContext;
        ContentBrowserPanel& contentBrowserPanel;

        std::string entityFilter;
        std::string fileFilter;
        std::filesystem::path currentFile;
        std::vector<std::filesystem::path> cachedFiles;
    };
}
