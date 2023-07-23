#pragma once

#include "editor_panel.h"

namespace fe {
    class ProjectSettingsPanel : public EditorPanel {
    public:
        explicit ProjectSettingsPanel(Editor& editor);
        ~ProjectSettingsPanel() override;

        void onImGui() override;

    private:
    };
}
