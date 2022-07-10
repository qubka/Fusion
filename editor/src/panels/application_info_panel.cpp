#include "application_info_panel.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/core/time.hpp"
#include "fusion/scene/scene_manager.hpp"
#include "fusion/filesystem/file_system.hpp"

using namespace fe;

ApplicationInfoPanel::ApplicationInfoPanel(Editor* editor) : EditorPanel{ICON_MDI_INFORMATION_VARIANT " Application Info##appinfo", "AppInfo", editor} {

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
            }*/

            if (ImGui::TreeNode("Filesystem")) {
                auto mounted = FileSystem::Get()->getMounted();
                for (auto& [phys, virt] : mounted) {
                    ImGui::TextUnformatted(virt.string().c_str());
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("%s", phys.string().c_str());
                }
                ImGui::TreePop();
            }

            ImGui::NewLine();
            ImGui::Text("FPS : %5.2i", Time::FramesPerSecond());
            ImGui::Text("Frame Time : %5.2f ms", Time::DeltaTime().asMilliseconds());
            //ImGui::NewLine();
            //ImGui::Text("Scene : %s", SceneManager::Get()->getCurrentScene()->getName().c_str());
            ImGui::TreePop();
        };
    }
    ImGui::End();
}
