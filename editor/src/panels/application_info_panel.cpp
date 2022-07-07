#include "application_info_panel.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/core/time.hpp"
#include "fusion/scene/scene_manager.hpp"

using namespace fe;

ApplicationInfoPanel::ApplicationInfoPanel() : EditorPanel{ICON_MDI_INFORMATION_VARIANT " Application Info", "AppInfo"} {

}

ApplicationInfoPanel::~ApplicationInfoPanel() {

}

void ApplicationInfoPanel::onImGui() {
    auto flags = ImGuiWindowFlags_NoCollapse;
    ImGui::Begin(name.c_str(), &enabled, flags);
    {
        if (ImGui::TreeNode("Application")) {
            /*auto modules = Engine::Get()->getModules();

            if (ImGui::TreeNode("Modules")) {
                modules->onImGui();
                ImGui::TreePop();
            }

            auto renderGraph = Graphics::Get().getRenderGraph();
            if (ImGui::TreeNode("RenderGraph")) {
                renderGraph->onImGui();
                ImGui::TreePop();
            }*/

            //ImGui::NewLine();
            ImGui::Text("FPS : %5.2i", Time::FramesPerSecond());
            ImGui::Text("Frame Time : %5.2f ms", Time::DeltaTime().asMilliseconds());
            ImGui::NewLine();
            ImGui::Text("Scene : %s", SceneManager::Get()->getCurrentScene()->getName().c_str());
            ImGui::TreePop();
        };
    }
    ImGui::End();
}
