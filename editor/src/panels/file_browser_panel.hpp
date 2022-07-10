#pragma once

#include "editor_panel.hpp"

namespace ImGui {
    class FileBrowser;
}

namespace fe {
    class FileBrowserPanel : public EditorPanel {
    public:
        explicit FileBrowserPanel(Editor* editor);
        ~FileBrowserPanel() override;

        void onImGui() override;

        void open();
        bool isOpened();

        void setCurrentPath(const  fs::path& path);
        void setOpenDirectory(bool value);
        void setCallback(const std::function<void(const fs::path&)>& func) { callback = func; }

        void setFileTypeFilters(const std::vector<std::string>& formats);
        void clearFileTypeFilters();

    private:
        std::function<void(const fs::path&)> callback;
        ImGui::FileBrowser* fileBrowser;
    };
}
