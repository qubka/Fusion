#pragma once

#include "editor_panel.hpp"

namespace fe {
    struct DirectoryInfo {
        std::shared_ptr<DirectoryInfo> parent;
        std::vector<std::shared_ptr<DirectoryInfo>> children;
        fs::path path;
        std::string name;
        const char* icon;
        bool isDirectory;

        explicit DirectoryInfo(fs::path filepath);
    };

    class ContentBrowserPanel : public EditorPanel {
    public:
        explicit ContentBrowserPanel(Editor* editor);
        ~ContentBrowserPanel() override;

        void onImGui() override;
        void onNewProject() override;

        void refresh();

        void changeDirectory(const std::shared_ptr<DirectoryInfo>& directory);

    private:
        void drawFolder(const std::shared_ptr<DirectoryInfo>& dirInfo, bool defaultOpen = false);
        bool drawFile(size_t dirIndex, bool folder, int shownIndex, bool gridView);
        void drawBottom();
        //void drawBreadCrumbs();

        const fs::path& processDirectory(const fs::path& path, const std::shared_ptr<DirectoryInfo>& parent);
        bool moveFile(const fs::path& filepath, const fs::path& movepath);

        fs::path basePath;
        //fs::path movePath;

        bool isLocked{ false };
        bool isInListView{ false };
        bool updateBreadCrumbs{ false };
        bool updateNavigationPath{ true };
        int gridItemsPerRow{ 0 };
        float gridSize{ 128.0f };

        ImGuiTextFilter filter;

        std::shared_ptr<DirectoryInfo> currentDirectory;
        std::shared_ptr<DirectoryInfo> previousDirectory;
        std::shared_ptr<DirectoryInfo> nextDirectory;
        std::shared_ptr<DirectoryInfo> baseDirectory;
        std::unordered_map<fs::path, std::shared_ptr<DirectoryInfo>> directories;
        std::vector<std::shared_ptr<DirectoryInfo>> breadCrumbData;
    };
}
