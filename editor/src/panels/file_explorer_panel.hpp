#pragma once

namespace fe {
    class FileExplorerPanel {
    public:
        FileExplorerPanel() = default;
        ~FileExplorerPanel() = default;

        void onImGui();

    private:
        uint32_t currentNode{ UINT32_MAX };
    };
}
