#pragma once

#include "editor_panel.hpp"

namespace fe {
    class GameViewPanel : public EditorPanel {
    public:
        explicit GameViewPanel(Editor* editor);
        ~GameViewPanel() override;

        void onImGui() override;

    private:
        void drawToolBar();
    };
}
