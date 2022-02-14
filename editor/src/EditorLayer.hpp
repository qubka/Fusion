#pragma once

#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"

#include "Fusion/Core/Layer.hpp"
#include "Fusion/Input/BaseInput.hpp"
#include "Fusion/Scene/Scene.hpp"
#include "Fusion/Scene/SceneSerializer.hpp"
#include "Fusion/Renderer/EditorCamera.hpp"
#include "Fusion/Systems/MeshRenderer.hpp"
#include "Fusion/Utils/ProcessInfo.hpp"

namespace Fusion {
    class FUSION_API EditorLayer : public Layer {
    public:
        EditorLayer(Vulkan& vulkan, Renderer& renderer);
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

        ProcessInfo info{static_cast<unsigned int>(getpid())};

        // Panels
        SceneHierarchyPanel sceneHierarchyPanel;
        ContentBrowserPanel contentBrowserPanel;

        bool viewportFocused{false};
        bool viewportHovered{false};
        glm::vec2 viewportSize{};
        glm::vec2 viewportBounds[2]{};
        int gizmoType = -1;

        //std::shared_ptr<Texture> texture;

        enum class SceneState {
            Edit = 0, Play = 1
        };
        SceneState sceneState = SceneState::Edit;

        Vulkan& vulkan;
        Renderer& renderer;
        MeshRenderer meshRenderer;
    };
}
