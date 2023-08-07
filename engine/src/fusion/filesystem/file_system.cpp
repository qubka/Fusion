#include "file_system.h"
#include "storage.h"

#include "fusion/core/engine.h"

#if FUSION_PLATFORM_ANDROID
#include <android/asset_manager.h>
#include "fusion/devices/device_manager.h"
#endif

using namespace fe;

FileSystem::FileSystem() {
}

FileSystem::~FileSystem() {
}

void FileSystem::ReadBytes(const fs::path& filepath, const std::function<void(gsl::span<const std::byte>)>& handler) {
    if (!FileSystem::IsExists(filepath)) {
        FS_LOG_ERROR("File: '{}' not exist!", filepath);
        return;
    }

    auto storage = Storage::ReadFile(filepath);
    handler(*storage);
}

std::string FileSystem::ReadText(const fs::path& filepath) {
    if (!FileSystem::IsExists(filepath)) {
        FS_LOG_ERROR("File: '{}' not exist!", filepath);
        return {};
    }

    auto storage = Storage::ReadFile(filepath);
    return { (char*)storage->data(), storage->size() };

    /*std::ifstream is{filepath, std::ios::in};

    if (!is.is_open()) {
        FS_LOG_ERROR("File: '{}' could not be opened!", filepath);
        return {};
    }

    std::stringstream ss;
    std::string line;

    while (!is.eof()) {
        getline(is, line);
        ss << line << '\n';
    }
    return ss.str();*/
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

    if (FileSystem::IsExists(root) && fs::is_directory(root)) {
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

bool FileSystem::IsExists(const fs::path& filepath) {
#ifdef FUSION_PLATFORM_ANDROID
    auto assetManager = static_cast<AAssetManager*>(DeviceManager::Get()->getNativeManager());
    auto dir = AAssetManager_openDir(assetManager, filepath.parent_path().string().c_str());

    std::string filename{ filepath.filename() };

    bool found = false;

    const char* name = nullptr;
    while ((name = AAssetDir_getNextFileName(dir)) != nullptr) {
        if (filename == name) {
            found = true;
            break;
        }
    }

    AAssetDir_close(dir);

    return found;
#else
    return fs::exists(filepath);
#endif
}