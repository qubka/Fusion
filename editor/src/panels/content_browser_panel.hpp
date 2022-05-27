#pragma once

namespace fe {
    class ContentBrowserPanel {
    public:
        ContentBrowserPanel() : currentDirectory{getAssetPath()} {};
        ~ContentBrowserPanel() = default;

        void onImGui();

        const std::filesystem::path& getCurrentFile() const { return currentFile; }
        const std::filesystem::path& getCurrentDirectory() const { return currentDirectory; }

        void selectFile(const std::filesystem::path& path) {
            currentDirectory = path.parent_path();
            currentFile = path;
        }

    private:
        std::filesystem::path currentDirectory;
        std::filesystem::path currentFile;
    };
}
