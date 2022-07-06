#pragma once

#include "editor_panel.hpp"

namespace ImGui {
    class FileBrowser;
}

namespace fe {
    class FileBrowserPanel : public EditorPanel {
    public:
        FileBrowserPanel();
        ~FileBrowserPanel() override;

        void onImGui() override;

        void open();
        void setCurrentPath(const  std::filesystem::path& path);
        void setOpenDirectory(bool value);
        void setCallback(const std::function<void(const std::filesystem::path&)>& func) {
            callback = func;
        }

    private:
        std::function<void(const std::filesystem::path&)> callback;
        ImGui::FileBrowser* fileBrowser;
    };
}
