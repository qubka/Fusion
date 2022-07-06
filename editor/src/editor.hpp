#pragma once

#include "fusion/core/app.hpp"

#include "panels/editor_panel.hpp"
#include "panels/file_browser_panel.hpp"

namespace fe {
    class Camera;
    class Scene;

    class Editor : public App {
    public:
        Editor(const std::string& name, const Version& version);
        ~Editor() override;

        void onStart() override;
        void onUpdate() override;
        void onImGui() override;

    private:
        void drawMenuBar();

        void BeginDockSpace(bool gameFullScreen);
        void EndDockSpace();

        void newScene();
        void openScene();
        void openScene(const std::filesystem::path& file);
        void saveScene();
        void saveSceneAs();

        void onScenePlay();
        void onSceneStop();
        void onViewportResize();

        void UI_Toolbar();

        std::shared_ptr<Scene> activeScene;
        std::shared_ptr<Camera> editorCamera;

        // Panels
        //ContentBrowserPanel contentBrowserPanel;
        //SceneHierarchyPanel sceneHierarchyPanel{ contentBrowserPanel };

        // Gizmo
        int gizmoType{ -1 };
        enum SceneState { Edit = 0, Play = 1 };
        SceneState sceneState{ Edit };

        glm::vec2 viewportSize{ -1.0f };
        //glm::mat4 viewportOrtho{ 1.0f };
        bool viewportFocused{ false };
        bool viewportHovered{ false };

        FileBrowserPanel fileBrowserPanel;

        //EditorSettings settings;
        std::vector<std::unique_ptr<EditorPanel>> panels;
        std::unordered_map<std::type_index, std::string> componentIconMap;
    };
}
