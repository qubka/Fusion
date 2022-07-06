#include "file_browser_panel.hpp"

#include <imgui-filebrowser/imfilebrowser.h>

using namespace fe;

FileBrowserPanel::FileBrowserPanel() : EditorPanel{"FileBrowserWindow", "FileBrowser"} {
    fileBrowser = new ImGui::FileBrowser(ImGuiFileBrowserFlags_CreateNewDir | ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_NoModal);
    fileBrowser->SetTitle("Test File Browser");
   // fileBrowser->SetTypeFilters({ ".cpp" , ".hpp" });
    fileBrowser->SetLabels(ICON_MDI_FOLDER, ICON_MDI_FILE, ICON_MDI_FOLDER_PLUS, ICON_MDI_AUTORENEW);
    fileBrowser->SetPwd();
}

FileBrowserPanel::~FileBrowserPanel() {
    delete fileBrowser;
}

void FileBrowserPanel::onImGui() {
    fileBrowser->Display();

    if (fileBrowser->HasSelected()) {
        callback(fileBrowser->GetSelected());

        fileBrowser->ClearSelected();
    }
}

void FileBrowserPanel::setCurrentPath(const std::filesystem::path& path) {
    fileBrowser->SetPwd(path);
}

void FileBrowserPanel::open() {
    fileBrowser->Open();
}

void FileBrowserPanel::setOpenDirectory(bool value) {
    auto flags = fileBrowser->GetFlags();
    if (value) {
        flags |= ImGuiFileBrowserFlags_SelectDirectory;
    } else {
        flags &= ~(ImGuiFileBrowserFlags_SelectDirectory);
    }
    fileBrowser->SetFlags(flags);
}