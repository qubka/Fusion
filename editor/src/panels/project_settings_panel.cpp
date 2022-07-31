#include "project_settings_panel.hpp"
#include "editor.hpp"

#include "fusion/devices/device_manager.hpp"

using namespace fe;

ProjectSettingsPanel::ProjectSettingsPanel(Editor* editor) : EditorPanel{ICON_MDI_APPLICATION " Project Settings###projectsettings", "Project Settings", editor} {

}

ProjectSettingsPanel::~ProjectSettingsPanel() {

}

void ProjectSettingsPanel::onImGui() {
    auto flags = ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowSize(ImVec2{640, 480}, ImGuiCond_FirstUseEver);
    ImGui::Begin(title.c_str(), &active, flags);
    {
        auto window = DeviceManager::Get()->getWindow(0);

        ImGui::Columns(2);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{2, 2});

        auto& projectSettings = editor->getProjectSettings();
        ImGuiUtils::PropertyText("Project Version", projectSettings.projectVersion.c_str());
        ImGuiUtils::PropertyText("Project Name", projectSettings.projectName.c_str());
        ImGuiUtils::PropertyText("Project Root", projectSettings.projectRoot.string().c_str());
        if (ImGuiUtils::PropertyText("Title", projectSettings.title))
            window->setTitle(projectSettings.title);
        const uint32_t width = projectSettings.size.x;
        ImGuiUtils::Property("Window Width", width);
        const uint32_t height = projectSettings.size.y;
        ImGuiUtils::Property("Window Height", height);
        if (ImGuiUtils::Property("Fullscreen", projectSettings.isFullscreen))
            window->setFullscreen(projectSettings.isFullscreen, nullptr);
        if (ImGuiUtils::Property("VSync", projectSettings.isVSync))
            window->setVSync(projectSettings.isVSync);
        if (ImGuiUtils::Property("Borderless", projectSettings.isBorderless))
            window->setBorderless(projectSettings.isVSync);
        if (ImGuiUtils::Property("Resizable", projectSettings.isResizable))
            window->setResizable(projectSettings.isResizable);
        if (ImGuiUtils::Property("Floating", projectSettings.isFloating))
            window->setFloating(projectSettings.isVSync);
        if (ImGuiUtils::Property("Show Console", projectSettings.isShowConsole))
            editor->showConsole();
        ImGui::PopStyleVar();
        ImGui::Columns(1);
    }
    ImGui::End();
}
