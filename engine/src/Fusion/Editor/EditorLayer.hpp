#pragma once

#include "EditorCamera.hpp"

#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"

#include "Fusion/Core/Layer.hpp"
#include "Fusion/Scene/Scene.hpp"
#include "Fusion/Scene/SceneSerializer.hpp"
#include "Fusion/Input/BaseInput.hpp"
#include "Fusion/Utils/ProcessInfo.hpp"

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

        void UI_Toolbar();

        std::shared_ptr<Scene> activeScene;
        EditorCamera editorCamera;

        ProcessInfo info{static_cast<unsigned int>(getpid())};

        // Panels
        SceneHierarchyPanel sceneHierarchyPanel;
        ContentBrowserPanel contentBrowserPanel;

        bool viewportFocused{false};
        bool viewportHovered{false};
        glm::vec2 viewportSize{};
        glm::vec2 viewportBounds[2]{};
        bool firstFrame{false};
        int gizmoType = -1;
        float camDistance;

        enum class SceneState {
            Edit = 0, Play = 1
        };
        SceneState sceneState = SceneState::Edit;
    };
}
