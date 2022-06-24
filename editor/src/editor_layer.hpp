#pragma once

#include "panels/scene_hierarchy_panel.hpp"
#include "panels/content_browser_panel.hpp"

#include "fusion/core/layer.hpp"
//#include "fusion/input/base_input.hpp"
#include "fusion/scene/scene.hpp"
#include "fusion/scene/scene_serializer.hpp"
#include "fusion/graphics/editor_camera.hpp"
#include "fusion/graphics/renderer.hpp"

namespace fe {
    class EditorLayer : public Layer {
    public:
        EditorLayer();
        ~EditorLayer() override;

        void onAttach() override;
        void onDetach() override;
        void onUpdate(float dt) override;
        void onRender(Renderer& renderer) override;
        void onImGui() override;

    private:
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
        std::shared_ptr<Scene> editorScene;
        std::filesystem::path editorScenePath;
        EditorCamera editorCamera;

        // Panels
        ContentBrowserPanel contentBrowserPanel;
        SceneHierarchyPanel sceneHierarchyPanel{ contentBrowserPanel };

        int gizmoType{ -1 };
        enum SceneState { Edit = 0, Play = 1 };
        SceneState sceneState{ Edit };

        glm::vec2 viewportSize{ -1.0f };
        glm::mat4 viewportOrtho{ 1.0f };
        bool viewportFocused{ false };
        bool viewportHovered{ false };
    };
}
