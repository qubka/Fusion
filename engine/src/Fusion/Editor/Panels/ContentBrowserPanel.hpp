#pragma once

namespace Fusion {
    class FUSION_API ContentBrowserPanel {
    public:
        ContentBrowserPanel();
        ~ContentBrowserPanel() = default;

        void onImGui();
    private:
        std::filesystem::path currentDirectory;
    };
}
