#pragma once

#include "EditorCamera.hpp"

#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"

#include "Fusion/Core/Layer.hpp"
#include "Fusion/Scene/Scene.hpp"

namespace Fusion {
    class FUSION_API EditorLayer : public Layer {
    public:
        EditorLayer();
        ~EditorLayer() override;

        void onAttach() override;
        void onDetach() override;
        void onUpdate() override;
        void onImGui() override;

    private:
        void newScene();
        void openScene();
        void saveSceneAs();

        std::shared_ptr<Scene> activeScene;
        EditorCamera editorCamera;

        // Panels
        SceneHierarchyPanel sceneHierarchyPanel;
        ContentBrowserPanel contentBrowserPanel;
    };
}
