#pragma once

#include "editor_panel.hpp"

namespace fe {
    class SceneViewPanel : public EditorPanel {
    public:
        explicit SceneViewPanel(Editor* editor);
        ~SceneViewPanel() override;

        void onImGui() override;
        //void onNewScene(Scene* scene) override;

        //void toolBar();
        //void drawGizmos(float width, float height, float xpos, float ypos, Scene* scene);

        //voidresize(uint32_t width, uint32_t height);

    private:
        std::unordered_map<std::type_index, bool> showComponentGizmoMap;

        //bool showStats{ false };
        //SharedPtr<Graphics::Texture2D> m_GameViewTexture = nullptr;
        //Scene* currentScene{ nullptr };
        //uint32_t m_Width, m_Height;
    };
}
