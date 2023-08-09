#include "pc_virtual_file_system.h"

#include "fusion/core/engine.h"

#include <physfs.h>

using namespace fe::pc;

VirtualFileSystem::VirtualFileSystem() : fe::VirtualFileSystem{} {
    const auto& args = Engine::Get()->getCommandLineArgs();
    PHYSFE_init(args[0].first.c_str());
}

VirtualFileSystem::~VirtualFileSystem() {
    PHYSFE_deinit();
}

void VirtualFileSystem::mount(const fs::path& path, const fs::path& mount) {
    if (auto it = mountPoints.find(path); it != mountPoints.end()) {
        if (it->second == mount) {
            return;
        } else {
            PHYSFE_unmount(path.string().c_str());
            mountPoints.erase(it);
        }
    }

    PHYSFE_mount(path.string().c_str(), mount.string().c_str(), 1);
    mountPoints.emplace(path, mount);
}

void VirtualFileSystem::unmount(const fs::path& path) {
    if (auto it = mountPoints.find(path); it != mountPoints.end()) {
        PHYSFE_unmount(path.string().c_str());
        mountPoints.erase(it);
    }
}

/*const std::map<fs::path, fs::path>& VirtualFileSystem::getMounted() const {
    auto sp = PHYSFE_getSearchPath();

    std::vector<fs::path> files;

    for (auto i = sp; *i; i++)
        files.emplace_back(*i);

    PHYSFE_freeList(sp);
    return files;
    //return mountPoints;
}*/

void VirtualFileSystem::readBytes(const fs::path& filepath, const std::function<void(gsl::span<const std::byte>)>& handler) const {
    auto fsFile = PHYSFE_openRead(filepath.string().c_str());

    if (!fsFile) {
        FE_LOG_ERROR("Failed to open file: {} - {}", filepath, PHYSFE_getErrorByCode(PHYSFE_getLastErrorCode()));
        return;
    }

    auto size = PHYSFE_fileLength(fsFile);
    std::vector<std::byte> data(size);
    PHYSFE_readBytes(fsFile, data.data(), static_cast<PHYSFE_uint64>(size));

    if (PHYSFE_close(fsFile) == 0) {
        FE_LOG_ERROR("Failed to close file: {} - {}", filepath, PHYSFE_getErrorByCode(PHYSFE_getLastErrorCode()));
    }

    handler(data);
}

std::string VirtualFileSystem::readText(const fs::path& filepath) const {
    auto fsFile = PHYSFE_openRead(filepath.string().c_str());

    if (!fsFile) {
        FE_LOG_ERROR("Failed to open file: {} - {}", filepath, PHYSFE_getErrorByCode(PHYSFE_getLastErrorCode()));
        return {};
    }

    auto size = PHYSFE_fileLength(fsFile);
    std::vector<uint8_t> data(size);
    PHYSFE_readBytes(fsFile, data.data(), static_cast<PHYSFE_uint64>(size));

    if (PHYSFE_close(fsFile) == 0) {
        FE_LOG_ERROR("Failed to close file: {} - {}", filepath, PHYSFE_getErrorByCode(PHYSFE_getLastErrorCode()));
    }

    return { data.begin(), data.end() };
}

bool VirtualFileSystem::writeBytes(const fs::path& filepath, gsl::span<const std::byte> buffer) const {
    return false;
}

bool VirtualFileSystem::writeText(const fs::path& filepath, std::string_view text) const {
    return false;
}

bool VirtualFileSystem::isExists(const fs::path& filepath) const {
    return PHYSFE_exists(filepath.string().c_str()) != 0;
}

bool VirtualFileSystem::isDirectory(const fs::path& filepath) const {
    PHYSFE_Stat stat;
    if (!PHYSFE_stat(filepath.string().c_str(), &stat)) {
        return false;
    } else {
        if (stat.filetype == PHYSFE_FILETYPE_SYMLINK) {
            // PHYSFE_stat() doesn't follow symlinks, so we do it manually
            const char* realdir = PHYSFE_getRealDir(filepath.string().c_str());
            if (realdir == nullptr) {
                return false;
            } else {
                return isDirectory(fs::path{realdir} / filepath);
            }
        } else {
            return stat.filetype == PHYSFE_FILETYPE_DIRECTORY;
        }
    }
}

std::vector<fs::path> VirtualFileSystem::getFiles(const fs::path& filepath, bool recursive, std::string_view ext) const {
    auto rc = PHYSFE_enumerateFiles(filepath.string().c_str());

    std::vector<fs::path> files;

    for (auto i = rc; *i; i++) {
        fs::path path { *i };
        if (recursive && isDirectory(path)) {
            auto filesInFound = getFiles(path, recursive);
            files.insert(files.end(), filesInFound.begin(), filesInFound.end());
        } else {
            if (ext.empty() || ext == path.filename().string())
                files.push_back(std::move(path));
        }
    }

    PHYSFE_freeList(rc);
    return files;
}