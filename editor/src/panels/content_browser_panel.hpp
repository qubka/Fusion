#pragma once

namespace fe {
    class ContentBrowserPanel {
    public:
        ContentBrowserPanel() { selectDirectory(getAssetPath()); }
        ~ContentBrowserPanel() = default;

        void onImGui();

        const std::fs::path& getCurrentFile() const { return currentFile; }
        const std::fs::path& getCurrentDirectory() const { return currentDirectory; }

        void selectFile(const std::fs::path& file);
        void selectDirectory(const std::fs::path& dir);

    private:
        void drawFileExplorer();
        void drawContentBrowser();

        std::fs::path currentDirectory;
        std::fs::path currentFile;
        std::vector<std::fs::path> currentFiles;

        std::string filter;
        std::vector<std::fs::path> filteredFiles;
    };
}
