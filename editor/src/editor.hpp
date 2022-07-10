#pragma once

#include "fusion/core/default_application.hpp"

#include "panels/editor_panel.hpp"
#include "panels/file_browser_panel.hpp"

#include <entt/entity/entity.hpp>

namespace fe {
    class Camera;
    class Scene;

    struct EditorSettings {
        bool fullScreenOnPlay{ false };
        bool fullScreenSceneView{ false };
        bool showImGuiDemo{ false };
        ImGuiUtils::Theme theme{ ImGuiUtils::Theme::ClassicDark };

        /*float gridSize{ 10.0f };
        uint32_t debugDrawFlags{ 0 };
        uint32_t physics2DDebugFlags{ 0 };
        uint32_t physics3DDebugFlags{ 0 };
        bool showGrid{ true };
        bool showGizmos{ true };
        bool showViewSelected{ false };
        bool snapQuizmo{ false };
        bool showImGuiDemo{ true };
        bool view2D{ false };
        bool sleepOutofFocus{ true };
        bool freeAspect{ true };
        bool halfRes{ false };
        float snapAmount{ 1.0f };
        float imGuizmoScale{ 0.25f };
        float fixedAspect{ 1.0f };
        float aspectRatio{ 1.0f };*/
    };

    enum class EditorState {
        Paused,
        Play,
        Next,
        Preview
    };

    class Editor : public DefaultApplication {
    public:
        explicit Editor(std::string name);
        ~Editor() override;

        void onStart() override;
        void onUpdate() override;
        void onImGui() override;

        void openFile();

        void fileOpenCallback(const fs::path& path);
        void projectOpenCallback(const fs::path& path);
        void newProjectOpenCallback(const fs::path& path);
        void newProjectLocationCallback(const fs::path& path);

        const EditorSettings& getSettings() { return editorSettings; }
        const EditorState& getState() { return editorState; }

    private:
        void beginDockSpace(bool gameFullScreen);
        void endDockSpace();
        void drawMenuBar();

        EditorSettings editorSettings;
        EditorState editorState{ EditorState::Paused };

        FileBrowserPanel fileBrowserPanel{ this };
        std::vector<std::unique_ptr<EditorPanel>> panels;
        std::unordered_map<std::type_index, std::string> componentIconMap;

        entt::entity selectedEntity{ entt::null };
        entt::entity copiedEntity{ entt::null };
        bool cutCopyEntity{ false };

        fs::path projectLocation{ };
        bool reopenNewProjectPopup{ false };
        bool newProjectPopupOpen{ false };
        bool locationPopupOpened{ false };
    };
}
