#include "scene_view_panel.hpp"

#include "fusion/scene/scene_manager.hpp"
#include "fusion/graphics/graphics.hpp"

using namespace fe;

SceneViewPanel::SceneViewPanel(Editor* editor) : EditorPanel{ICON_MDI_GAMEPAD_VARIANT " Scene###scene", "Scene", editor} {
    //showComponentGizmoMap[typeid(LightComponent)] = true;
    //showComponentGizmoMap[typeid(CameraComponent)] = true;
    //showComponentGizmoMap[typeid(SoundComponent).] = true;
}

SceneViewPanel::~SceneViewPanel() {

}

void SceneViewPanel::onImGui() {

    auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    auto scene = SceneManager::Get()->getScene();
    if (!ImGui::Begin(title.c_str(), &active, flags) || !scene) {
        ImGui::End();
        return;
    }

    auto offscreen = Graphics::Get()->getAttachment("offscreen");
    if (!offscreen) {
        ImGui::End();
        return;
    }


    ImGuiUtils::Image((Image2d*) offscreen, { 100, 100 }, true);


    ImGui::End();
}
