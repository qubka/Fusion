#pragma once

#include "fusion/core/app.hpp"

#include "panels/scene_hierarchy_panel.hpp"
#include "panels/content_browser_panel.hpp"

namespace fe {
    class Camera;
    class Scene;
    class EditorApp : public App {
    public:
        EditorApp(const std::string& name, const Version& version);
        ~EditorApp() override;

        void start() override;
        void update() override;

    private:
        void newScene();
        void openScene();
        void openScene(const std::filesystem::path& file);
        void saveScene();
        void saveSceneAs();

        void onScenePlay();
        void onSceneStop();
        void onViewportResize();

        void onImGui();
        void UI_Toolbar();

        std::shared_ptr<Scene> activeScene;
        std::shared_ptr<Camera> editorCamera;

        // Panels
        ContentBrowserPanel contentBrowserPanel;
        SceneHierarchyPanel sceneHierarchyPanel{ contentBrowserPanel };

        // Gizmo
        int gizmoType{ -1 };
        enum SceneState { Edit = 0, Play = 1 };
        SceneState sceneState{ Edit };

        glm::vec2 viewportSize{ -1.0f };
        //glm::mat4 viewportOrtho{ 1.0f };
        bool viewportFocused{ false };
        bool viewportHovered{ false };
    };
}
