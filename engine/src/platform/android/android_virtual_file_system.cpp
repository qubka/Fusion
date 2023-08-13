#include "android_virtual_file_system.h"
#include "android_engine.h"
#include "android.h"

using namespace fe::android;

VirtualFileSystem::VirtualFileSystem() : fe::VirtualFileSystem{} {
    auto app = static_cast<struct android_app*>(Engine::Get()->getNativeApp());
    assetManager = app->activity->assetManager;
}

VirtualFileSystem::~VirtualFileSystem() {

}

void VirtualFileSystem::mount(const fs::path& path, const fs::path& mount) {

}

void VirtualFileSystem::unmount(const fs::path& path) {

}

void VirtualFileSystem::readBytes(const fs::path& filepath, const std::function<void(gsl::span<const std::byte>)>& handler) const {
    auto asset = AAssetManager_open(assetManager, filepath.string().c_str(), AASSET_MODE_BUFFER);

    if (!asset) {
        FE_LOG_ERROR("Failed to open asset: {}", filepath);
        return;
    }

    handler({ (std::byte*)AAsset_getBuffer(asset), static_cast<size_t>(AAsset_getLength(asset)) });

    AAsset_close(asset);
}

std::string VirtualFileSystem::readText(const fs::path& filepath) const {
    auto asset = AAssetManager_open(assetManager, filepath.string().c_str(), AASSET_MODE_BUFFER);

    if (!asset) {
        FE_LOG_ERROR("Failed to open asset: {}", filepath);
        return {};
    }

    std::string data{ (char*)AAsset_getBuffer(asset), static_cast<size_t>(AAsset_getLength(asset)) };

    AAsset_close(asset);

    return data;
}

bool VirtualFileSystem::writeBytes(const fs::path& filepath, gsl::span<const std::byte> buffer) const {
    return false;
}

bool VirtualFileSystem::writeText(const fs::path& filepath, std::string_view text) const {
    return false;
}

bool VirtualFileSystem::isExists(const fs::path& filepath) const {
    if (filepath.empty())
        return false;

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
}

bool VirtualFileSystem::isDirectory(const fs::path& filepath) const {
    auto dir = AAssetManager_openDir(assetManager, filepath.string().c_str());

    bool found = false;

    if (AAssetDir_getNextFileName(dir) != nullptr) {
        found = true;
    }

    AAssetDir_close(dir);

    return found;
}

std::vector<fs::path> VirtualFileSystem::getFiles(const fs::path& filepath, bool recursive, std::string_view ext) const {
    auto dir = AAssetManager_openDir(assetManager, filepath.string().c_str());

    std::vector<fs::path> files;

    const char* name = nullptr;
    while ((name = AAssetDir_getNextFileName(dir)) != nullptr) {
        fs::path path{ filepath / name };
        if (recursive && isDirectory(path)) {
            auto filesInFound = getFiles(path, recursive);
            files.insert(files.end(), filesInFound.begin(), filesInFound.end());
        } else {
            if (ext.empty() || ext == FileSystem::GetExtension(path))
                files.push_back(std::move(path));
        }
    }

    AAssetDir_close(dir);
    return files;
}