#include "file_system.h"

#if FUSION_PLATFORM_ANDROID
#include "platform/android/android_virtual_file_system.h"
#elif FUSION_PLATFORM_LINUX || FUSION_PLATFORM_WINDOWS || FUSION_PLATFORM_APPLE
#include "platform/pc/pc_virtual_file_system.h"
#else
#pragma error("Unknown platform!");
#endif

using namespace fe;

FileSystem::FileSystem() {
#if FUSION_VIRTUAL_FS
#if FUSION_PLATFORM_ANDROID
    vfs = std::make_unique<android::VirtualFileSystem>();
#elif FUSION_PLATFORM_LINUX || FUSION_PLATFORM_WINDOWS || FUSION_PLATFORM_APPLE
    vfs = std::make_unique<pc::VirtualFileSystem>();
#else
    FE_LOG_FATAL("Unknown platform!");
#endif
#endif
}

FileSystem::~FileSystem() {
}

void FileSystem::ReadBytes(const fs::path& filepath, const std::function<void(gsl::span<const std::byte>)>& handler) {
#if FUSION_VIRTUAL_FS
        ModuleInstance->vfs->readBytes(filepath, handler);
#else
        std::ifstream is{filepath, std::ios::binary};

        if (!is.is_open()) {
            FE_LOG_ERROR("File: '{}' could not be opened", filepath);
            return;
        }

        // Stop eating new lines in binary mode!!!
        is.unsetf(std::ios::skipws);

        std::streampos size;
        is.seekg(0, std::ios::end);
        size = is.tellg();
        is.seekg(0, std::ios::beg);

        std::vector<uint8_t> buffer;
        buffer.reserve(size);
        buffer.insert(buffer.begin(), std::istream_iterator<uint8_t>(is), std::istream_iterator<uint8_t>());

        handler({ reinterpret_cast<std::byte*>(buffer.data()), buffer.size() });
#endif
}

std::string FileSystem::ReadText(const fs::path& filepath) {
#if FUSION_VIRTUAL_FS
        return ModuleInstance->vfs->readText(filepath);
#else
        std::ifstream is{filepath, std::ios::binary};

        if (!is.is_open()) {
            FE_LOG_ERROR("File: '{}' could not be opened", filepath);
            return {};
        }

        // Stop eating new lines in binary mode!!!
        is.unsetf(std::ios::skipws);

        return { std::istream_iterator<int8_t>(is), std::istream_iterator<int8_t>() };
#endif
}

bool FileSystem::WriteBytes(const fs::path& filepath, gsl::span<const std::byte> buffer) {
#if FUSION_VIRTUAL_FS
        return ModuleInstance->vfs->writeBytes(filepath, buffer);
#else
        std::ofstream os{filepath, std::ios::binary};
        os.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        return true;
#endif
}

bool FileSystem::WriteText(const fs::path& filepath, std::string_view text) {
#if FUSION_VIRTUAL_FS
        return ModuleInstance->vfs->writeText(filepath, text);
#else
        std::ofstream os{filepath, std::ios::binary};
        os.write(text.data(), text.size());
        return true;
#endif
}

std::string FileSystem::GetExtension(const fs::path& filepath) {
    return String::Lowercase(filepath.extension().string());
}

bool FileSystem::IsExists(const fs::path& filepath) {
#if FUSION_VIRTUAL_FS
        return ModuleInstance->vfs->isExists(filepath);
#else
        return fs::exists(filepath);
#endif
}

bool FileSystem::IsDirectory(const fs::path& filepath) {
#if FUSION_VIRTUAL_FS
        return ModuleInstance->vfs->isDirectory(filepath);
#else
        return fs::is_directory(filepath);
#endif
}

std::vector<fs::path> FileSystem::GetFiles(const fs::path& root, bool recursive, std::string_view ext) {
#if FUSION_VIRTUAL_FS
        return ModuleInstance->vfs->getFiles(root, recursive, ext);
#else
        std::vector<fs::path> paths;

        if (fs::exists(root) && fs::is_directory(root)) {
            if (!ext.empty()) {
                for (auto const& entry : fs::recursive_directory_iterator(root)) {
                    const auto& path = entry.path();
                    if (fs::is_regular_file(entry) && FileSystem::GetExtension(path) == ext)
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
#endif
}