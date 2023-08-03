#include "file_system.h"
#include "storage.h"

#include "fusion/core/engine.h"

using namespace fe;

FileSystem::FileSystem() {
}

FileSystem::~FileSystem() {
}

void FileSystem::ReadBytes(const fs::path& filepath, const std::function<void(gsl::span<const std::byte>)>& handler) {
    if (!fs::exists(filepath)) {
        LOG_ERROR << "File: \"" << filepath << "\" not exist!";
        return;
    }

    auto storage = Storage::ReadFile(filepath);
    handler(*storage);
}

std::string FileSystem::ReadText(const fs::path& filepath) {
    if (!fs::exists(filepath)) {
        LOG_ERROR << "File: \"" << filepath << "\" not exist!";
        return {};
    }

    std::ifstream is{filepath, std::ios::in};

    if (!is.is_open()) {
        LOG_ERROR << "File: \"" << filepath << "\" could not be opened!";
        return {};
    }

    std::stringstream ss;
    std::string line;

    while (!is.eof()) {
        getline(is, line);
        ss << line << '\n';
    }
    return ss.str();
}

bool FileSystem::WriteBytes(const fs::path& filepath, gsl::span<const std::byte> buffer) {
    std::ofstream os{filepath, std::ios::binary};
    os.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    return true;
}

bool FileSystem::WriteText(const fs::path& filepath, std::string_view text) {
    std::ofstream os{filepath, std::ios::binary};
    os.write(text.data(), text.size());
    return true;
}

std::string FileSystem::GetExtension(const fs::path& filepath) {
    return String::Lowercase(filepath.extension().string());
}

std::vector<fs::path> FileSystem::GetFilesInPath(const fs::path& root, const std::string& ext) {
    std::vector<fs::path> paths;

    if (fs::exists(root) && fs::is_directory(root)) {
        if (!ext.empty()) {
            for (auto const& entry : fs::recursive_directory_iterator(root)) {
                const auto& path = entry.path();
                if (fs::is_regular_file(entry) && path.extension() == ext)
                    paths.push_back(path);
            }
        } else {
            for (auto const& entry : fs::recursive_directory_iterator(root)) {
                if (fs::is_regular_file(entry))
                    paths.push_back(entry.path());
            }
        }
    }

    return paths;
}
