#pragma once

#include "editor_panel.hpp"

namespace fe {
    class ApplicationInfoPanel : public EditorPanel {
    public:
        ApplicationInfoPanel();
        ~ApplicationInfoPanel();

        void onImGui() override;

    private:
    };
}
