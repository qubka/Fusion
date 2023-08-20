#include "pc_virtual_file_system.h"

#include "fusion/core/engine.h"

#include <physfs.h>

using namespace fe::pc;

VirtualFileSystem::VirtualFileSystem() : fe::VirtualFileSystem{} {
    const auto& args = Engine::Get()->getCommandLineArgs();
    PHYSFS_init(args[0].first.c_str());
}

VirtualFileSystem::~VirtualFileSystem() {
    PHYSFS_deinit();
}

void VirtualFileSystem::mount(const fs::path& path, const fs::path& mount) {
    if (auto it = mountPoints.find(path); it != mountPoints.end()) {
        if (it->second == mount) {
            return;
        } else {
            PHYSFS_unmount(path.string().c_str());
            mountPoints.erase(it);
        }
    }

    PHYSFS_mount(path.string().c_str(), mount.string().c_str(), 1);
    mountPoints.emplace(path, mount);
}

void VirtualFileSystem::unmount(const fs::path& path) {
    if (auto it = mountPoints.find(path); it != mountPoints.end()) {
        PHYSFS_unmount(path.string().c_str());
        mountPoints.erase(it);
    }
}

/*const std::map<fs::path, fs::path>& VirtualFileSystem::getMounted() const {
    auto sp = PHYSFS_getSearchPath();

    std::vector<fs::path> files;

    for (auto i = sp; *i; i++)
        files.emplace_back(*i);

    PHYSFS_freeList(sp);
    return files;
    //return mountPoints;
}*/

void VirtualFileSystem::readBytes(const fs::path& filepath, const std::function<void(gsl::span<const uint8_t>)>& handler) const {
    auto fsFile = PHYSFS_openRead(filepath.string().c_str());

    if (!fsFile) {
        FE_LOG_ERROR("Failed to open file: {} - {}", filepath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        return;
    }

    auto size = PHYSFS_fileLength(fsFile);
    std::vector<uint8_t> buffer(size);
    PHYSFS_readBytes(fsFile, buffer.data(), static_cast<PHYSFS_uint64>(size));

    if (PHYSFS_close(fsFile) == 0) {
        FE_LOG_ERROR("Failed to close file: {} - {}", filepath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    }

    handler(buffer);
}

std::string VirtualFileSystem::readText(const fs::path& filepath) const {
    auto fsFile = PHYSFS_openRead(filepath.string().c_str());

    if (!fsFile) {
        FE_LOG_ERROR("Failed to open file: {} - {}", filepath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        return {};
    }

    auto size = PHYSFS_fileLength(fsFile);
    std::vector<uint8_t> buffer(size);
    PHYSFS_readBytes(fsFile, buffer.data(), static_cast<PHYSFS_uint64>(size));

    if (PHYSFS_close(fsFile) == 0) {
        FE_LOG_ERROR("Failed to close file: {} - {}", filepath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    }

    return { buffer.begin(), buffer.end() };
}

bool VirtualFileSystem::writeBytes(const fs::path& filepath, gsl::span<const uint8_t> buffer) const {
    return false;
}

bool VirtualFileSystem::writeText(const fs::path& filepath, std::string_view text) const {
    return false;
}

bool VirtualFileSystem::isExists(const fs::path& filepath) const {
    return PHYSFS_exists(filepath.string().c_str()) != 0;
}

bool VirtualFileSystem::isDirectory(const fs::path& filepath) const {
    PHYSFS_Stat stat;
    if (!PHYSFS_stat(filepath.string().c_str(), &stat)) {
        return false;
    } else {
        if (stat.filetype == PHYSFS_FILETYPE_SYMLINK) {
            // PHYSFS_stat() doesn't follow symlinks, so we do it manually
            const char* realdir = PHYSFS_getRealDir(filepath.string().c_str());
            if (realdir == nullptr) {
                return false;
            } else {
                return isDirectory(fs::path{realdir} / filepath);
            }
        } else {
            return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
        }
    }
}

std::vector<fs::path> VirtualFileSystem::getFiles(const fs::path& filepath, bool recursive, std::string_view ext) const {
    auto rc = PHYSFS_enumerateFiles(filepath.string().c_str());

    std::vector<fs::path> files;

    for (auto i = rc; *i; i++) {
        fs::path path { *i };
        if (recursive && isDirectory(path)) {
            auto filesInFound = getFiles(path, recursive);
            files.insert(files.end(), filesInFound.begin(), filesInFound.end());
        } else {
            if (ext.empty() || ext == FileSystem::GetExtension(path))
                files.push_back(std::move(path));
        }
    }

    PHYSFS_freeList(rc);
    return files;
}