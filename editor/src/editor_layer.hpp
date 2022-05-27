#pragma once

#include "panels/scene_hierarchy_panel.hpp"
#include "panels/content_browser_panel.hpp"
#include "panels/file_explorer_panel.hpp"

#include "fusion/core/layer.hpp"
#include "fusion/input/base_input.hpp"
#include "fusion/scene/scene.hpp"
#include "fusion/scene/scene_serializer.hpp"
#include "fusion/renderer/editor_camera.hpp"
#include "fusion/utils/process_info.hpp"
#include "fusion/renderer/renderer.hpp"

namespace fe {
    class EditorApp;
    class EditorLayer : public Layer {
    public:
        EditorLayer(EditorApp& context);
        ~EditorLayer() override;

        void onAttach() override;
        void onDetach() override;
        void onUpdate(float dt) override;
        void onRender(Renderer& renderer) override;
        void onImGui() override;

    private:
        void newScene();
        void openScene();
        void openScene(const std::filesystem::path& path);
        void saveSceneAs();

        void UI_Toolbar();

        std::shared_ptr<Scene> activeScene;
        EditorCamera editorCamera;

        // Panels
        FileExplorerPanel fileExplorerPanel;
        ContentBrowserPanel contentBrowserPanel;
        SceneHierarchyPanel sceneHierarchyPanel{ contentBrowserPanel };

        int gizmoType{ -1 };
        enum class SceneState { Edit = 0, Play = 1 };
        SceneState sceneState = SceneState::Edit;

        glm::vec2 viewportSize;
        bool viewportFocused;
        bool viewportHovered;

        EditorApp& context;
    };
}
