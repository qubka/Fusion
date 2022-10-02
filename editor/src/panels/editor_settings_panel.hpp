#pragma once

#include "editor_panel.hpp"

namespace fe {
    class EditorSettingsPanel : public EditorPanel {
    public:
        explicit EditorSettingsPanel(Editor& editor);
        ~EditorSettingsPanel() override;

        void onImGui() override;

    private:
    };
}