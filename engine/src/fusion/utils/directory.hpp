#pragma once

namespace fe {
    class Directory {
    public:
        Directory() = delete;

        static std::vector<std::filesystem::path> GetFiles(const std::filesystem::path& dir, const std::string& filter = "", const std::vector<std::string>& formats = {});

        static std::vector<std::filesystem::path> GetFilesRecursive(const std::filesystem::path& dir, const std::string& filter = "", const std::vector<std::string>& formats = {});

        static bool HasDirectories(const std::filesystem::path& dir);
    };
}
