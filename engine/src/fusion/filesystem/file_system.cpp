#include "file_system.hpp"
#include "storage.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/utils/string.hpp"

#include <physfs.h>

using namespace fe;

FileSystem::FileSystem() {
    const auto& args = Engine::Get()->getCommandLineArgs();
    PHYSFS_init(args[0].first.c_str());
    //PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);
}

FileSystem::~FileSystem() {
    PHYSFS_deinit();
}

bool FileSystem::Mount(const fs::path& path, const fs::path& mount) {
    int result =  PHYSFS_mount(path.string().c_str(), mount.string().c_str(), 1);
    if (!result) {
        LOG_ERROR << "Failed to mount directory or archive: " << path << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }
    return result != 0;
}

bool FileSystem::Unmount(const fs::path& path) {
    int result = PHYSFS_unmount(path.string().c_str());
    if (!result) {
        LOG_ERROR << "Failed to create directory or archiv: " << path << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }
    return result != 0;
}

void FileSystem::onUpdate() {

}

std::vector<fs::path> FileSystem::getMounted() {
    auto sp = PHYSFS_getSearchPath();

    std::vector<fs::path> files;

    for (auto i = sp; *i; i++)
        files.emplace_back(*i);

    PHYSFS_freeList(sp);
    return files;
}

bool FileSystem::Exists(const fs::path& path) {
    return PHYSFS_exists(path.string().c_str()) != 0;
}

bool FileSystem::CreateDirectory(const fs::path& path) {
    int result = PHYSFS_mkdir(path.string().c_str());
    if (!result) {
        LOG_ERROR << "Failed to create directory: " << path << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }
    return result != 0;
}

bool FileSystem::SetWriteDirectory(const fs::path& path) {
    int result = PHYSFS_setWriteDir(path.string().c_str());
    if (!result) {
        LOG_ERROR << "Failed to set directory to write: " << path << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }
    return result != 0;
}

void FileSystem::Read(const fs::path& filepath, const FileSystem::SimpleHandler& handler) {
    auto fsFile = PHYSFS_openRead(filepath.string().c_str());

    if (!fsFile) {
        if (!fs::exists(filepath) || !fs::is_regular_file(filepath)) {
            LOG_ERROR << "Failed to open file: " << filepath << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
            return;
        }

        auto storage = Storage::readFile(filepath);
        handler(storage->getData(), storage->getSize());

        LOG_INFO << "Reading text with default filesystem implementation: " << filepath;

        return;
    }

    auto size = PHYSFS_fileLength(fsFile);
    std::vector<uint8_t> data(size);
    PHYSFS_readBytes(fsFile, data.data(), static_cast<PHYSFS_uint64>(size));

    if (PHYSFS_close(fsFile) == 0) {
        LOG_ERROR << "Failed to close file: " << filepath << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    handler(data.data(), data.size());
}

std::vector<uint8_t> FileSystem::ReadBytes(const fs::path& filepath) {
    auto fsFile = PHYSFS_openRead(filepath.string().c_str());

    if (!fsFile) {
        if (!fs::exists(filepath) || !fs::is_regular_file(filepath)) {
            LOG_ERROR << "Failed to open file: " << filepath << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
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
        LOG_ERROR << "Failed to close file: " << filepath << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
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
        LOG_ERROR << "Failed to close file: " << filepath << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    return { data.begin(), data.end() };
}

bool FileSystem::Write(const fs::path& filepath, const void* buffer, size_t size) {
    auto fsFile = PHYSFS_openWrite(filepath.string().c_str());

    if (!fsFile) {
        std::ofstream os{filepath, std::ios::binary};
        os.write(reinterpret_cast<const char*>(buffer), size);
        return true;
    }

    bool ret = false;

    auto written = PHYSFS_writeBytes(fsFile, buffer, size);
    if(written != size)
        LOG_ERROR << "Failed to write in file: " << filepath << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    else
        ret = written;

    if (PHYSFS_close(fsFile) == 0) {
        LOG_ERROR << "Failed to close file: " << filepath << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    return ret;
}

std::vector<fs::path> FileSystem::GetFiles(const fs::path& path, bool recursive) {
    auto rc = PHYSFS_enumerateFiles(path.string().c_str());

    std::vector<fs::path> files;

    for (auto i = rc; *i; i++) {
        if (recursive && IsDirectory(*i)) {
            auto filesInFound = GetFiles(*i, recursive);
            files.insert(files.end(), filesInFound.begin(), filesInFound.end());
        } else {
            files.emplace_back(*i);
        }
    }

    PHYSFS_freeList(rc);
    return files;
}

bool FileSystem::IsDirectory(const fs::path& path) {
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
                return IsDirectory(fs::path{realdir} / path);
            }
        } else {
            return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
        }
    }
}

FileStats FileSystem::GetStats(const fs::path& path) {
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

bitmask::bitmask<FileAttributes> FileSystem::GetAttributes(const fs::path& path) {
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

std::string FileSystem::GetExtension(const fs::path& path) {
    return String::Lowercase(path.extension().string());
}