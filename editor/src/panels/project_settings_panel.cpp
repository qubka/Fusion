#include "project_settings_panel.h"
#include "editor.h"

#include "fusion/devices/device_manager.h"
#include "fusion/scripting/script_engine.h"

using namespace fe;

ProjectSettingsPanel::ProjectSettingsPanel(Editor& editor) : EditorPanel{ICON_MDI_APPLICATION " Project Settings###projectsettings", "Project Settings", editor} {

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

        static ImGuiTextFilter filter;
        static fs::path selected;
        static std::vector<fs::path> files;

        auto& projectSettings = editor.getProjectSettings();
        ImGuiUtils::PropertyText("Project Version", projectSettings.projectVersion.c_str());
        ImGuiUtils::PropertyText("Project Name", projectSettings.projectName.c_str());
        ImGuiUtils::PropertyText("Project Root", projectSettings.projectRoot.string().c_str());
#if FUSION_SCRIPTING
        if (ImGuiUtils::PropertyFile("Script Module Path", projectSettings.scriptModulePath, selected, filter, files, projectSettings.projectRoot, ".dll")) {
            if (!projectSettings.scriptModulePath.empty()) {
                auto scriptEngine = ScriptEngine::Get();
                scriptEngine->setAssemblyPaths(FUSION_ASSET_PATH "scripts/Fusion-ScriptCore.dll", projectSettings.projectRoot / projectSettings.scriptModulePath);
                scriptEngine->reloadAssembly();
            }
        }
#endif

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
            editor.showConsole();
        ImGui::PopStyleVar();
        ImGui::Columns(1);
    }
    ImGui::End();
}
