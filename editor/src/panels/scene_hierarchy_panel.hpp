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
        template<typename Component>
        void drawComponent(const std::string& label, entt::entity entity, std::function<void(Component&)>&& function);
        template<typename Component>
        void drawComponentMenuItem(const std::string& label, entt::entity entity);

        bool drawFileBrowser(const std::string& label, std::string& value, const std::vector<std::string>& formats, float columnWidth = 150.0f);
        static bool drawVec2Control(const std::string& label, glm::vec2& values, float minValue = 0.0f, float maxValue = 0.0f, float resetValue = 0.0f, float columnWidth = 150.0f);
        static bool drawVec3Control(const std::string& label, glm::vec3& values, float minValue = 0.0f, float maxValue = 0.0f, float resetValue = 0.0f, float columnWidth = 150.0f);
        static bool drawValueControl(const std::string& label, std::function<bool()>&& function, float columnWidth = 150.0f);
        template<typename Enum>
        static bool drawEnumControl(const std::string& label, Enum& value, float columnWidth = 150.0f);

        enum ComponentMode { None, MoveUp, MoveDown, Reset, Remove, Copy, Paste };

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
