#pragma once

namespace fe {
    class ContentBrowserPanel {
    public:
        ContentBrowserPanel() : currentDirectory{getAssetPath()} {};
        ~ContentBrowserPanel() = default;

        void onImGui();

        const std::filesystem::path& getCurrentFile() const { return currentFile; }
        const std::filesystem::path& getCurrentDirectory() const { return currentDirectory; }

        void selectFile(const std::filesystem::path& file) {
            currentDirectory = file.parent_path();
            currentFile = file;
        }

    private:
        void drawFileExplorer();
        void drawContentBrowser();

        std::filesystem::path currentDirectory;
        std::filesystem::path currentFile;
        uint32_t currentNode{ UINT32_MAX };
    };
}
