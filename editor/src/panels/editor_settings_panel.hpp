#pragma once

#include "editor_panel.hpp"

namespace fe {
    class EditorSettingsPanel : public EditorPanel {
    public:
        EditorSettingsPanel(Editor* editor);
        ~EditorSettingsPanel();

        void onImGui() override;

    private:
    };
}