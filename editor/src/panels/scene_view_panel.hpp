#pragma once

#include "editor_panel.hpp"

#include <volk/volk.h>

namespace fe {
    class SceneViewPanel : public EditorPanel {
    public:
        explicit SceneViewPanel(Editor* editor);
        ~SceneViewPanel() override;

        void onImGui() override;
        //void onNewScene(Scene* scene) override;

        //void drawGizmos(float width, float height, float xpos, float ypos, Scene* scene);

        void drawToolBar();

    private:
        std::unordered_map<std::type_index, bool> showComponentGizmoMap;
    };
}
