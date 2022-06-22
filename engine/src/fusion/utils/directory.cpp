#include "directory.hpp"

using namespace fe;

std::vector<std::filesystem::path> Directory::GetFiles(const std::filesystem::path& dir, const std::string& filter, const std::vector<std::string>& formats) {
    std::vector<std::filesystem::path> paths;

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        const auto& path = entry.path();

        if (!filter.empty() && entry.is_directory() && String::FindInsensitive(path.filename().string(), filter) == std::string::npos)
            continue;

        if (!formats.empty() && std::find(formats.begin(), formats.end(), path.filename().extension().string()) == formats.end())
            continue;

        paths.push_back(path);
    }

    std::sort(paths.begin(), paths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b) {
        return is_directory(a) && !is_directory(b);
    });

    return paths;
}

std::vector<std::filesystem::path> Directory::GetFilesRecursive(const std::filesystem::path& dir, const std::string& filter, const std::vector<std::string>& formats) {
    std::vector<std::filesystem::path> paths;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
        if (entry.is_directory())
            continue;

        const auto& path = entry.path();

        if (!filter.empty() && String::FindInsensitive(path.filename().string(), filter) == std::string::npos)
            continue;

        if (!formats.empty() && std::find(formats.begin(), formats.end(), path.filename().extension().string()) == formats.end())
            continue;

        paths.push_back(path);
    }

    std::sort(paths.begin(), paths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b) {
        return a < b;
    });

    return paths;
}

bool Directory::HasDirectories(const std::filesystem::path& dir) {
    if (is_directory(dir) && !is_empty(dir)) {
        for (const auto& entry: std::filesystem::directory_iterator(dir)) {
            if (entry.is_directory())
                return true;
        }
    }
    return false;
}
