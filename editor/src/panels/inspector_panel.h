#pragma once

#include "editor_panel.h"

#include "fusion/imgui/imgui_entity_editor.h"

namespace fe {
    class InspectorPanel : public EditorPanel {
    public:
        explicit InspectorPanel(Editor& editor);
        ~InspectorPanel() override;

        void onImGui() override;

        void setDebugMode(bool mode) { debugMode = mode; };
        bool isDebugMode() const { return debugMode; };

    private:
        ImGui::EntityEditor<entt::entity> enttEditor;
        bool debugMode = false;
    };
}
