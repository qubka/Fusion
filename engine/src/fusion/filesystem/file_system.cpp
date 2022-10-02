#include "file_system.hpp"
#include "storage.hpp"

#include "fusion/core/engine.hpp"

using namespace fe;

FileSystem::FileSystem() {
}

FileSystem::~FileSystem() {
}

void FileSystem::ReadBytes(const fs::path& filepath, const std::function<void(std::span<const std::byte>)>& handler) {
    auto storage = Storage::ReadFile(filepath);
    handler(*storage);
}

std::string FileSystem::ReadText(const fs::path& filepath) {
    std::ifstream is{filepath, std::ios::in};

    if (!is.is_open()) {
        throw std::runtime_error("File " + filepath.string() + " could not be opened");
    }

    std::stringstream ss;
    std::string line;

    while (!is.eof()) {
        getline(is, line);
        ss << line << '\n';
    }
    return ss.str();
}

bool FileSystem::WriteBytes(const fs::path& filepath, std::span<const std::byte> buffer) {
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