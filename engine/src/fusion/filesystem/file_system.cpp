#include "file_system.hpp"
#include "storage.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/utils/string.hpp"

using namespace fe;

#if PHYSFS
#include <physfs.h>

FileSystem::FileSystem() {
    PHYSFS_init(Engine::Get()->getCommandLineArgs()[0].first.c_str());
}

FileSystem::~FileSystem() {
    PHYSFS_deinit();
}

bool FileSystem::addSearchPath(const fs::path& path, const std::string& mount, bool append) {
    if (std::find(searchPaths.begin(), searchPaths.end(), path) != searchPaths.end())
        return false;

    if (PHYSFS_mount(path.c_str(), mount.empty() ? nullptr : mount.c_str(), append) == 0) {
        LOG_WARNING << "Failed to mount path \"" << path << "\" - " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
        return false;
    }

    searchPaths.emplace_back(path);
    return true;
}

bool FileSystem::removeSearchPath(const fs::path& path) {
    auto it = std::find(searchPaths.begin(), searchPaths.end(), path);

    if (it == searchPaths.end())
        return false;

    if (PHYSFS_unmount(path.string().c_str()) == 0) {
        LOG_WARNING << "Failed to unmount path \"" << path << "\" - " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
        return false;
    }

    searchPaths.erase(it);
    return true;
}

void FileSystem::clearSearchPath() {
    for (const auto& searchPath : searchPaths) {
        if (PHYSFS_unmount(searchPath.string().c_str()) == 0) {
            LOG_WARNING << "Failed to unmount path \"" << searchPath << "\" - "  << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
        }
    }

    searchPaths.clear();
}

bool FileSystem::ExistsInPath(const fs::path& path) {
    if (PHYSFS_isInit() == 0) return false;

    return PHYSFS_exists(path.string().c_str()) != 0;
}

bool FileSystem::CreateDirectoryInPath(const fs::path& path) {
    if (PHYSFS_mkdir(path.string().c_str()) == 0) {
        LOG_WARNING << "Failed to create directory: \"" << path << "\" - " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
        return false;
    }
    return true;
}

bool FileSystem::SetWriteDirectoryInPath(const fs::path& path) {
    if (PHYSFS_setWriteDir(path.string().c_str()) == 0) {
        LOG_WARNING << "Failed to set directory to write: \"" << path << "\" - " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
        return false;
    }
    return true;
}

void FileSystem::Read(const fs::path& filepath, const FileSystem::SimpleHandler& handler) {
    auto fsFile = PHYSFS_openRead(filepath.string().c_str());

    if (!fsFile) {
        if (!fs::exists(filepath) || !fs::is_regular_file(filepath)) {
            LOG_ERROR << "Failed to open file: \"" << filepath << "\" - " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
            return;
        }

        auto storage = Storage::readFile(filepath);
        handler(storage->getData(), storage->getSize());

        LOG_INFO << "Reading text with default filesystem implementation: \"" << filepath << "\"";

        return;
    }

    auto size = PHYSFS_fileLength(fsFile);
    std::vector<uint8_t> data(size);
    PHYSFS_readBytes(fsFile, data.data(), static_cast<PHYSFS_uint64>(size));

    if (PHYSFS_close(fsFile) == 0) {
        LOG_ERROR << "Failed to close file: \"" << filepath << "\" - " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    handler(data.data(), data.size());
}

std::vector<uint8_t> FileSystem::ReadBytes(const fs::path& filepath) {
    auto fsFile = PHYSFS_openRead(filepath.string().c_str());

    if (!fsFile) {
        if (!fs::exists(filepath) || !fs::is_regular_file(filepath)) {
            LOG_ERROR << "Failed to open file: \"" << filepath << "\" - " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
            return {};
        }

        auto storage = Storage::readFile(filepath);
        std::vector<uint8_t> data(storage->getSize());
        std::memcpy(data.data(), storage->getData(), storage->getSize());

        LOG_INFO << "Reading file with default filesystem implementation: " << filepath;

        return data;
    }

    auto size = PHYSFS_fileLength(fsFile);
    std::vector<uint8_t> data(size);
    PHYSFS_readBytes(fsFile, data.data(), static_cast<PHYSFS_uint64>(size));

    if (PHYSFS_close(fsFile) == 0) {
        LOG_ERROR << "Failed to close file: \"" << filepath << "\" - " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    return data;
}

std::string FileSystem::ReadText(const fs::path& filepath) {
    auto fsFile = PHYSFS_openRead(filepath.string().c_str());

    if (!fsFile) {
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

        LOG_INFO << "Reading file with default filesystem implementation: " << filepath;

        return ss.str();
    }

    auto size = PHYSFS_fileLength(fsFile);
    std::vector<uint8_t> data(size);
    PHYSFS_readBytes(fsFile, data.data(), static_cast<PHYSFS_uint64>(size));

    if (PHYSFS_close(fsFile) == 0) {
        LOG_ERROR << "Failed to close file: \"" << filepath << "\" - " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    return { data.begin(), data.end() };
}

bool FileSystem::WriteBytes(const fs::path& filepath, const void* buffer, size_t size) {
    auto fsFile = PHYSFS_openWrite(filepath.string().c_str());

    if (!fsFile) {
        std::ofstream os{filepath, std::ios::binary};
        os.write(reinterpret_cast<const char*>(buffer), size);
        return true;
    }

    bool ret = false;

    auto written = PHYSFS_writeBytes(fsFile, buffer, size);
    if(written != size)
        LOG_ERROR << "Failed to write in file: \"" << filepath << "\" - " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    else
        ret = written;

    if (PHYSFS_close(fsFile) == 0) {
        LOG_ERROR << "Failed to close file: \"" << filepath << "\" - " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    return ret;
}

bool FileSystem::WriteText(const fs::path& filepath, const std::string& text) {
    return WriteBytes(filepath, text.data(), text.length());
}

std::vector<fs::path> FileSystem::GetFilesInPath(const fs::path& path, bool recursive) {
    auto rc = PHYSFS_enumerateFiles(path.string().c_str());

    std::vector<fs::path> files;

    for (auto i = rc; *i; i++) {
        if (recursive && IsDirectoryInPath(*i)) {
            auto filesInFound = GetFilesInPath(*i, recursive);
            files.insert(files.end(), filesInFound.begin(), filesInFound.end());
        } else {
            files.emplace_back(*i);
        }
    }

    PHYSFS_freeList(rc);
    return files;
}

bool FileSystem::IsDirectoryInPath(const fs::path& path) {
    PHYSFS_Stat stat;
    if (!PHYSFS_stat(path.string().c_str(), &stat)) {
        return false;
    } else {
        if (stat.filetype == PHYSFS_FILETYPE_SYMLINK) {
            // PHYSFS_stat() doesn't follow symlinks, so we do it manually
            const char* realdir = PHYSFS_getRealDir(path.string().c_str());
            if (realdir == nullptr) {
                return false;
            } else {
                return IsDirectoryInPath(fs::path{realdir} / path);
            }
        } else {
            return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
        }
    }
}

FileStats FileSystem::GetStatsInPath(const fs::path& path) {
    PHYSFS_Stat stat = {};
    PHYSFS_stat(path.string().c_str(), &stat);

    return {
        static_cast<size_t>(stat.filesize),
        DateTime::Seconds(stat.modtime),
        DateTime::Seconds(stat.createtime),
        DateTime::Seconds(stat.accesstime),
        static_cast<FileType>(stat.filetype),
        static_cast<bool>(stat.readonly)
    };
}

bitmask::bitmask<FileAttributes> FileSystem::GetAttributesInPath(const fs::path& path) {
    bitmask::bitmask<FileAttributes> attributes;

    PHYSFS_Stat stat = {};
    PHYSFS_stat(path.string().c_str(), &stat);

    switch (stat.filetype) {
        case PHYSFS_FILETYPE_REGULAR:
            attributes |= FileAttributes::Regular;
            break;
        case PHYSFS_FILETYPE_DIRECTORY:
            attributes |= FileAttributes::Directory;
            break;
        case PHYSFS_FILETYPE_SYMLINK:
            attributes |= FileAttributes::Symlink;
            break;
    }

    if (stat.readonly)
        attributes |= FileAttributes::ReadOnly;

    return attributes;
}
#else
FileSystem::FileSystem() {
}

FileSystem::~FileSystem() {
}

void FileSystem::Read(const fs::path& filepath, const FileSystem::SimpleHandler& handler) {
    auto storage = Storage::readFile(filepath);
    handler(storage->getData(), storage->getSize());

    LOG_INFO << "Reading text with default filesystem implementation: " << filepath;
}

std::vector<uint8_t> FileSystem::ReadBytes(const fs::path& filepath) {
    auto storage = Storage::readFile(filepath);
    std::vector<uint8_t> data(storage->getSize());
    std::memcpy(data.data(), storage->getData(), storage->getSize());
    return data;
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

bool FileSystem::WriteBytes(const fs::path& filepath, const void* buffer, size_t size) {
    std::ofstream os{filepath, std::ios::binary};
    os.write(reinterpret_cast<const char*>(buffer), size);
    return true;
}

bool FileSystem::WriteText(const fs::path& filepath, const std::string& text) {
    return WriteBytes(filepath, text.data(), text.length());
}
#endif

std::string FileSystem::GetExtension(const fs::path& path) {
    return String::Lowercase(path.extension().string());
}