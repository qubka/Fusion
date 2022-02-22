#pragma once

namespace fe {
    class ContentBrowserPanel {
    public:
        ContentBrowserPanel();
        ~ContentBrowserPanel() = default;

        void onImGui();
    private:
        std::filesystem::path currentDirectory;
    };
}
