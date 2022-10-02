#pragma once

#include "editor_panel.hpp"

namespace fe {
    class ProjectSettingsPanel : public EditorPanel {
    public:
        explicit ProjectSettingsPanel(Editor& editor);
        ~ProjectSettingsPanel() override;

        void onImGui() override;

    private:
    };
}
