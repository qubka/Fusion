#include "scene_view_panel.hpp"

#include "fusion/scene/scene_manager.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/images/image2d.hpp"
#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/devices/device_manager.hpp"

using namespace fe;

SceneViewPanel::SceneViewPanel(Editor* editor) : EditorPanel{ICON_MDI_GAMEPAD_VARIANT " Scene###scene", "Scene", editor} {
    //showComponentGizmoMap[typeid(LightComponent)] = true;
    //showComponentGizmoMap[typeid(CameraComponent)] = true;
    //showComponentGizmoMap[typeid(SoundComponent).] = true;
}

SceneViewPanel::~SceneViewPanel() {

}

void SceneViewPanel::onImGui() {

    if (auto size = DeviceManager::Get()->getWindow(0)->getSize(); size != lastSize) {
        auto newSize = size;
        lastSize = size;
    }

    auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    auto scene = SceneManager::Get()->getScene();
    if (!ImGui::Begin(title.c_str(), &active, flags) || !scene) {
        ImGui::End();
        return;
    }

    auto offscreen = dynamic_cast<const Image2d*>(Graphics::Get()->getAttachment("offscreen"));
    if (!offscreen) {
        ImGui::End();
        return;
    }

    //auto bitmap = offscreen->getBitmap(0, 1);

   // Image::CopyImage(offscreen->getImage(), output->getImage(), output->getMemory(), offscreen->getFormat(), { lastSize.x, lastSize.y, 1 }, offscreen->getLayout(), 0, 1);
    //output = std::make_unique<Image2d>(bitmap->getSize(), VK_FORMAT_R8G8B8A8_UNORM);
    //output->setPixels(bitmap->getData<uint8_t>(), 0, 1);

    ImGuiUtils::Image(const_cast<Image2d*>(offscreen), { 100, 100 }, true);

    ImGui::End();
}
