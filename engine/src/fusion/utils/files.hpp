#pragma once

namespace fe {
    class fs {
    public:
        static std::vector<std::fs::path> walk(const std::fs::path& dir, const std::string& filter = "", const std::vector<std::string>& formats = {});
        static std::vector<std::fs::path> recursive_walk(const std::fs::path& dir, const std::string& filter = "", const std::vector<std::string>& formats = {});
        static bool has_directories(const std::fs::path& dir);
        static std::string extension_icon(const std::fs::path& file);

    private:
        static std::unordered_map<std::string, std::string> extensions;
    };
}