#pragma once

#include "fusion/core/default_application.hpp"
#include "fusion/graphics/cameras/controllers/editor_camera_controller.hpp"

#include "panels/editor_panel.hpp"
#include "panels/file_browser_panel.hpp"

namespace fe {
    class Camera;
    class Scene;

    struct EditorSettings {
        bool fullScreenOnPlay{ false };
        bool fullScreenSceneView{ false };
        bool showImGuiDemo{ false };
        bool showGizmos{ true };
        bool snapGizmos{ false };
        float snapAmount{ 0.5f };
        float snapBound{ 0.1f };
        uint32_t gizmosOperation{ UINT32_MAX };
        bool showGrid{ true };
        bool showStats{ false };
        bool muteAudio{ false };
        bool view2D{ false };
        bool freeAspect{ true };
        float fixedAspect{ 1.0f };
        float aspectRatio{ 1.0f };
        ImGuiUtils::Theme theme{ ImGuiUtils::Theme::ClassicDark };

        /*f
        uint32_t debugDrawFlags{ 0 };
        uint32_t physics2DDebugFlags{ 0 };
        uint32_t physics3DDebugFlags{ 0 };
        bool showViewSelected{ false };
        bool sleepOutofFocus{ true };
        float imGuizmoScale{ 0.25f };
        */
    };

    enum class EditorState : unsigned char {
        Paused,
        Play,
        Next,
        Preview
    };

    class Editor : public DefaultApplication {
    public:
        explicit Editor(std::string_view name);
        ~Editor() override;

        void openFile();

        // TODO: Rework
        void fileOpenCallback(const fs::path& path);
        void projectOpenCallback(const fs::path& path);
        void newProjectOpenCallback(const fs::path& path);
        void newProjectLocationCallback(const fs::path& path);

        void openTextFile(const fs::path& filepath, const std::function<void()>& callback);
        void removePanel(EditorPanel* panel);
        EditorPanel* getPanel(const std::string& name);

        std::shared_ptr<Camera> getCamera() { return editorCamera; }

        EditorSettings& getSettings() { return editorSettings; }
        EditorState& getState() { return editorState; }

        void setSelected(entt::entity entity) { selectedEntity = entity; }
        entt::entity getSelected() const { return selectedEntity; }

        void setCopiedEntity(entt::entity entity, bool cut = false) { copiedEntity = entity; cutCopyEntity = cut; }
        entt::entity getCopiedEntity() const { return copiedEntity; }
        bool getCutCopyEntity() const { return cutCopyEntity; }

        const std::unordered_map<type_index, std::string>& getComponentIconMap() const { return componentIconMap; }
        bool getComponentIcon(type_index type, std::string& out) const {
            if (auto icon = componentIconMap.find(type); icon != componentIconMap.end()) {
                out = icon->second;
                return true;
            }
            return false;
        }

        bool isSceneActive() const { return sceneActive; }
        void setSceneActive(bool flag) { sceneActive = flag; }
        bool isSceneViewActive() const { return sceneViewActive; }
        void setSceneViewActive(bool flag) { sceneViewActive = flag; }
        void setSceneViewSize(const glm::vec2& size) { sceneViewSize = size; }

        //void focusCamera(const glm::vec3& point, float distance, float speed = 1.0f);

        void onImGuizmo();
    private:
        void onStart() override;
        void onUpdate() override;
        void onImGui() override;

        void beginDockSpace(bool gameFullScreen);
        void endDockSpace();
        void drawMenuBar();

        EditorSettings editorSettings;
        EditorState editorState{ EditorState::Paused };

        EditorCameraController editorCameraController;
        std::shared_ptr<Camera> editorCamera;
        float currentSceneAspectRatio{ 0.0f };
        float cameraTransitionStartTime{ 0.0f };
        float cameraTransitionSpeed{ 0.0f };
        bool transitioningCamera{ false };
        glm::vec3 cameraDestination{ 0.0f };
        glm::vec3 cameraStartPosition{ 0.0f };

        FileBrowserPanel fileBrowserPanel{ this };
        std::vector<std::unique_ptr<EditorPanel>> panels;
        std::unordered_map<type_index, std::string> componentIconMap;

        entt::entity selectedEntity{ entt::null };
        entt::entity copiedEntity{ entt::null };
        bool cutCopyEntity{ false };

        bool reopenNewProjectPopup{ false };
        bool newProjectPopupOpen{ false };
        bool locationPopupOpened{ false };
        fs::path projectLocation;

        bool sceneActive{ false };
        bool sceneViewActive{ false };
        glm::vec2 sceneViewSize{ 0.0f };
        //glm::vec2 gameViewSize{ 0.0f };
    };
}
