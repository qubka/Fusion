#pragma once

namespace fe {
    class fs {
    public:
        static std::vector<std::filesystem::path> walk(const std::filesystem::path& dir, const std::string& filter = "", const std::vector<std::string>& formats = {});
        static std::vector<std::filesystem::path> recursive_walk(const std::filesystem::path& dir, const std::string& filter = "", const std::vector<std::string>& formats = {});
        static bool has_directories(const std::filesystem::path& dir);

        static const char* ICON_FA_FOLDER_CLOSE;
        static const char* ICON_FA_FOLDER_OPEN;
        static const char* ICON_FA_FOLDER_EMPTY;
        static const char* ICON_FA_FILE;
        static const char* ICON_FA_SEARCH;
        static const char* ICON_FA_REPLY;
        static const char* ICON_FA_PLAY;
        static const char* ICON_FA_STOP;
        static const char* ICON_FA_OK;
        static const char* ICON_FA_REMOVE;
        static const char* ICON_FA_AXIS;
        static const char* ICON_FA_CAMERA;
        static const char* ICON_FA_CUBE;
        static const char* ICON_FA_CUBES;
        static const char* ICON_FA_MANY_CUBES;
        static const char* ICON_FA_INFO;
        static const char* ICON_FA_LIST;
        static const char* ICON_FA_STATS;
        static const char* ICON_FA_ARCHIVE;
        static const char* ICON_FA_STORAGE;
        static const char* ICON_FA_COG;
        static const char* ICON_FA_COGS;
        static const char* ICON_FA_LOCK;
        static const char* ICON_FA_UNLOCK;

        static std::string extension_icon(const std::filesystem::path& file);

    private:
        static std::unordered_map<std::string, std::string> extensions;
    };
}