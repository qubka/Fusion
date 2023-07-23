#pragma once

#include "editor_panel.h"

namespace fe {
    class HierarchyPanel : public EditorPanel {
    public:
        explicit HierarchyPanel(Editor& editor);
        ~HierarchyPanel() override;

        void onImGui() override;

    private:
        void drawNode(entt::entity node, entt::registry& registry);

        ImGuiTextFilter hierarchyFilter;
        entt::entity doubleClicked;
        entt::entity hadRecentDroppedEntity;
        entt::entity currentPrevious;
        bool selectUp;
        bool selectDown;
    };
}
