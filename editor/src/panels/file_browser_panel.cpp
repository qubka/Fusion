#include "file_browser_panel.h"

using namespace fe;

FileBrowserPanel::FileBrowserPanel(Editor& editor) : EditorPanel{"File Browser Window", "FileBrowser", editor},
    fileBrowser{ ImGuiFileBrowserFlags_CreateNewDir | ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_NoModal } {
    fileBrowser.SetTitle("Test File Browser");
   // fileBrowser.SetTypeFilters({ ".cpp" , ".h" });
    fileBrowser.SetLabels(ICON_MDI_FOLDER, ICON_MDI_FILE, ICON_MDI_FOLDER_PLUS, ICON_MDI_AUTORENEW);
    fileBrowser.SetPwd();
}

FileBrowserPanel::~FileBrowserPanel() {
}

void FileBrowserPanel::onImGui() {
    fileBrowser.Display();

    if (fileBrowser.HasSelected()) {
        callback(fileBrowser.GetSelected());

        fileBrowser.ClearSelected();
    }
}

void FileBrowserPanel::setCurrentPath(const fs::path& path) {
    fileBrowser.SetPwd(path);
}

void FileBrowserPanel::open() {
    fileBrowser.Open();
}

bool FileBrowserPanel::isOpened() {
    return fileBrowser.IsOpened();
}

void FileBrowserPanel::setOpenDirectory(bool value) {
    auto flags = fileBrowser.GetFlags();
    if (value) {
        flags |= ImGuiFileBrowserFlags_SelectDirectory;
    } else {
        flags &= ~ImGuiFileBrowserFlags_SelectDirectory;
    }
    fileBrowser.SetFlags(flags);
}

void FileBrowserPanel::setFileTypeFilters(const std::vector<std::string>& formats) {
    fileBrowser.SetTypeFilters(formats);
}

void FileBrowserPanel::clearFileTypeFilters() {
    fileBrowser.SetTypeFilters({});
}