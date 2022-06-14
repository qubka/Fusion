#pragma once

namespace fe {
    class fs {
    public:
        static std::vector<std::filesystem::path> walk(const std::filesystem::path& dir, const std::string& filter = "", const std::vector<std::string>& formats = {});
        static std::vector<std::filesystem::path> recursive_walk(const std::filesystem::path& dir, const std::string& filter = "", const std::vector<std::string>& formats = {});
        static bool has_directories(const std::filesystem::path& dir);
        static std::string extension_icon(const std::filesystem::path& file);

    private:
        static std::unordered_map<std::string, std::string> extensions;
    };
}