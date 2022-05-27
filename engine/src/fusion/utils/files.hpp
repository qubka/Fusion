#pragma once

namespace fe {
    class fs {
    public:
        static std::vector<std::filesystem::directory_entry> walk(const std::filesystem::path& path);

        static const char* ICON_FA_FOLDER;
        static const char* ICON_FA_FILE;
        static const char* ICON_FA_SEARCH;
        static const char* ICON_FA_REPLY;
        static const char* ICON_FA_PLAY;
        static const char* ICON_FA_STOP;

        static std::unordered_map<std::string, std::string> extentions;
    };
}