#pragma once

#include "panels/scene_hierarchy_panel.hpp"
#include "panels/content_browser_panel.hpp"

#include "fusion/core/layer.hpp"
#include "fusion/input/base_input.hpp"
#include "fusion/scene/scene.hpp"
#include "fusion/scene/scene_serializer.hpp"
#include "fusion/renderer/editor_camera.hpp"
//#include "fusion/systems/meshrenderer.hpp"
#include "fusion/utils/process_info.hpp"

#include <imgui.h>

namespace fe {
    class EditorApp;
    class EditorLayer : public Layer {
    public:
        EditorLayer(const EditorApp& app);
        ~EditorLayer() override;

        void onAttach() override;
        void onDetach() override;
        void onUpdate() override;
        void onRender() override;
        void onImGui() override;

    private:
        void newScene();
        void openScene();
        void openScene(const std::filesystem::path& path);
        void saveSceneAs();

        void UI_Toolbar();

        std::shared_ptr<Scene> activeScene;
        EditorCamera editorCamera;

        //ProcessInfo info{static_cast<unsigned int>(getpid())};

        // Panels
        SceneHierarchyPanel sceneHierarchyPanel;
        ContentBrowserPanel contentBrowserPanel;

        int gizmoType = -1;
        enum class SceneState {
            Edit = 0, Play = 1
        };
        SceneState sceneState = SceneState::Edit;

    public:
        ImTextureID offscreen;

        const EditorApp& app;
    };
}
