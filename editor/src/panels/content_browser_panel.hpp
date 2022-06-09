#pragma once

namespace fe {
    class ContentBrowserPanel {
    public:
        ContentBrowserPanel() { selectDirectory(getAssetPath()); }
        ~ContentBrowserPanel() = default;

        void onImGui();

        const std::filesystem::path& getCurrentFile() const { return currentFile; }
        const std::filesystem::path& getCurrentDirectory() const { return currentDirectory; }
        void selectFile(const std::filesystem::path& file);
        void selectDirectory(const std::filesystem::path& dir);

    private:
        void drawFileExplorer();
        void drawContentBrowser();

        std::filesystem::path currentDirectory;
        std::filesystem::path currentFile;
        std::vector<std::filesystem::path> currentFiles;

        std::string filter;
        std::vector<std::filesystem::path> cachedFiles;
    };
}
