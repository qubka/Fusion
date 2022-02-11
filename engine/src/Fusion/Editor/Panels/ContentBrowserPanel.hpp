#pragma once

namespace Fusion {
    class FUSION_API ContentBrowserPanel {
    public:
        ContentBrowserPanel(const std::filesystem::path& path);
        ~ContentBrowserPanel() = default;

        void onImGui();
    private:
        std::filesystem::path currentDirectory;
        std::filesystem::path root;

        //std::shared_ptr<Texture2D> directoryIcon;
        //std::shared_ptr<Texture2D> fileIcon;
    };
}
