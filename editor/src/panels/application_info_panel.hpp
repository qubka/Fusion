#pragma once

#include "editor_panel.hpp"

namespace fe {
    class ApplicationInfoPanel : public EditorPanel {
    public:
        explicit ApplicationInfoPanel(Editor& editor);
        ~ApplicationInfoPanel() override;

        void onImGui() override;

    private:
    };
}
