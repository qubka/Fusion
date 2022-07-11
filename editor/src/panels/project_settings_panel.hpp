#pragma once

#include "editor_panel.hpp"

namespace fe {
    class ProjectSettingsPanel : public EditorPanel {
    public:
        ProjectSettingsPanel(Editor* editor);
        ~ProjectSettingsPanel();

        void onImGui() override;

    private:
    };
}
