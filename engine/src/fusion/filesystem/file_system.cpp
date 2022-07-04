#include "file_system.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/filesystem/stream.hpp"
#include "fusion/utils/string.hpp"

#include <physfs.h>
#include <IconsFontAwesome4.h>

using namespace fe;

bool FileSystem::Exists(const std::filesystem::path& path) {
    auto pathStr = path.string();
    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');
    return PHYSFS_exists(pathStr.c_str()) != 0;
}

void FileSystem::Read(const std::filesystem::path& filename, const FileSystem::SimpleHandler& handler) {
    auto bytes = ReadBytes(filename);
    handler(bytes.data(), bytes.size());
}

std::vector<uint8_t> FileSystem::ReadBytes(const std::filesystem::path& filename) {
    /*IFileStream file{filename};
    file >> std::noskipws;

    auto fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> bytes;
    bytes.reserve(fileSize);

    file.insert(bytes.begin(), std::istream_iterator<uint8_t>(infile), std::istream_iterator<uint8_t>());

    std::copy(std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>(), back_inserter(bytes));

    file.close();

    return bytes;*/

    auto pathStr = filename.string();
    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');
    auto fsFile = PHYSFS_openRead(pathStr.c_str());

    if (!fsFile) {
        LOG_ERROR << "Failed to open file " << filename << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    auto size = PHYSFS_fileLength(fsFile);
    std::vector<uint8_t> data(size);
    PHYSFS_readBytes(fsFile, data.data(), static_cast<PHYSFS_uint64>(size));

    if (PHYSFS_close(fsFile) == 0) {
        LOG_ERROR << "Failed to close file " << filename << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    return data;
}

std::string FileSystem::ReadText(const std::filesystem::path& filename) {
    auto pathStr = filename.string();
    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');
    auto fsFile = PHYSFS_openRead(pathStr.c_str());

    if (!fsFile) {
        if (!std::filesystem::exists(filename) || !std::filesystem::is_regular_file(filename)) {
            LOG_ERROR << "Failed to open file " << filename << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
            return "";
        }

        std::ifstream is{filename};
        std::stringstream buffer;
        buffer << is.rdbuf();
        return buffer.str();
    }

    auto size = PHYSFS_fileLength(fsFile);
    std::vector<uint8_t> data(size);
    PHYSFS_readBytes(fsFile, data.data(), static_cast<PHYSFS_uint64>(size));

    if (PHYSFS_close(fsFile) == 0) {
        LOG_ERROR << "Failed to close file " << filename << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    return { data.begin(), data.end() };
}

std::vector<std::filesystem::path> FileSystem::GetFiles(const std::filesystem::path& path, bool recursive) {
    auto pathStr = path.string();
    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');
    auto rc = PHYSFS_enumerateFiles(pathStr.c_str());

    std::vector<std::filesystem::path> files;

    for (auto i = rc; *i; i++) {
        if (IsDirectory(*i)) {
            if (recursive) {
                auto filesInFound = GetFiles(*i, recursive);
                files.insert(files.end(), filesInFound.begin(), filesInFound.end());
            }
        } else {
            files.emplace_back(*i);
        }
    }

    PHYSFS_freeList(rc);
    return files;
}

std::vector<std::filesystem::path> FileSystem::GetFilesWithFilter(const std::filesystem::path& path, bool recursive, const std::string& filter, const std::vector<std::string>& formats) {
    auto pathStr = path.string();
    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');
    auto rc = PHYSFS_enumerateFiles(pathStr.c_str());

    std::vector<std::filesystem::path> files;

    for (auto i = rc; *i; i++) {
        if (IsDirectory(*i)) {
            if (recursive) {
                auto filesInFound = GetFilesWithFilter(*i, recursive, filter, formats);
                files.insert(files.end(), filesInFound.begin(), filesInFound.end());
            }
        } else {
            std::filesystem::path filename{*i};

            if (!filter.empty() && String::FindInsensitive(filename.string(), filter) == std::string::npos)
                continue;

            if (!formats.empty() && std::find(formats.begin(), formats.end(), GetExtension(filename)) == formats.end())
                continue;

            files.push_back(filename);
        }
    }

    PHYSFS_freeList(rc);
    return files;
}

bool FileSystem::HasDirectories(const std::filesystem::path& path) {
    if (IsDirectory(path)) {
        auto pathStr = path.string();
        auto rc = PHYSFS_enumerateFiles(pathStr.c_str());

        for (auto i = rc; *i; i++) {
            if (IsDirectory(*i))
                return true;
        }
    }
    return false;
}

bool FileSystem::IsDirectory(const std::filesystem::path& path) {
    auto pathStr = path.string();
    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

    PHYSFS_Stat stat;
    if (!PHYSFS_stat(pathStr.c_str(), &stat)) {
        return false;
    } else {
        if (stat.filetype == PHYSFS_FILETYPE_SYMLINK) {
            // PHYSFS_stat() doesn't follow symlinks, so we do it manually
            const char* realdir = PHYSFS_getRealDir(pathStr.c_str());
            if (realdir == nullptr) {
                return false;
            } else {
                const std::filesystem::path realfname = std::filesystem::path{realdir} / path;
                return IsDirectory(realfname);
            }
        } else {
            return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
        }
    }
}

FileStats FileSystem::GetStats(const std::filesystem::path& path) {
    auto pathStr = path.string();
    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

    PHYSFS_Stat stat;
    PHYSFS_stat(pathStr.c_str(), &stat);

    return {
        static_cast<size_t>(stat.filesize),
        DateTime::Seconds(stat.modtime),
        DateTime::Seconds(stat.createtime),
        DateTime::Seconds(stat.accesstime),
        static_cast<FileType>(stat.filetype),
        static_cast<bool>(stat.readonly)
    };
}

bitmask::bitmask<FileAttributes> FileSystem::GetAttributes(const std::filesystem::path& path) {
    bitmask::bitmask<FileAttributes> attributes;

    auto pathStr = path.string();
    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');
    PHYSFS_Stat stat;
    PHYSFS_stat(pathStr.c_str(), &stat);

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

std::string FileSystem::GetExtension(const std::filesystem::path& filename) {
    return String::Lowercase(filename.extension().string());
}

std::string FileSystem::GetIcon(const std::filesystem::path& filename) {
    auto key = Extensions.find(GetExtension(filename));
    return key != Extensions.end() ? key->second.c_str() : ICON_FA_FILE;
}

std::unordered_map<std::string, std::string> FileSystem::Extensions = {
    {".gif", ICON_FA_FILE_IMAGE_O},
    {".jpeg", ICON_FA_FILE_IMAGE_O},
    {".jpg", ICON_FA_FILE_IMAGE_O},
    {".png", ICON_FA_FILE_IMAGE_O},
    {".ktx", ICON_FA_FILE_IMAGE_O},

    {".pdf", ICON_FA_FILE_PDF_O},

    {".doc", ICON_FA_FILE_WORD_O},
    {".docx", ICON_FA_FILE_WORD_O},

    {".ppt", ICON_FA_FILE_POWERPOINT_O},
    {".pptx", ICON_FA_FILE_POWERPOINT_O},

    {".xls", ICON_FA_FILE_EXCEL_O},
    {".xlsx", ICON_FA_FILE_EXCEL_O},

    {".aac", ICON_FA_FILE_AUDIO_O},
    {".mp3", ICON_FA_FILE_AUDIO_O},
    {".ogg", ICON_FA_FILE_AUDIO_O},

    {".avi", ICON_FA_FILE_VIDEO_O},
    {".flv", ICON_FA_FILE_VIDEO_O},
    {".mkv", ICON_FA_FILE_VIDEO_O},
    {".mp4", ICON_FA_FILE_VIDEO_O},

    {".gz", ICON_FA_FILE_ARCHIVE_O},
    {".zip", ICON_FA_FILE_ARCHIVE_O},
    {".7z", ICON_FA_FILE_ARCHIVE_O},
    {".tar", ICON_FA_FILE_ARCHIVE_O},
    {".tar.bz2", ICON_FA_FILE_ARCHIVE_O},
    {".tar.Z", ICON_FA_FILE_ARCHIVE_O},
    {".tar.gz", ICON_FA_FILE_ARCHIVE_O},
    {".tar.lz4", ICON_FA_FILE_ARCHIVE_O},
    {".tar.lz", ICON_FA_FILE_ARCHIVE_O},
    {".tar.lzma", ICON_FA_FILE_ARCHIVE_O},
    {".tar.xz", ICON_FA_FILE_ARCHIVE_O},
    {".tar.zst", ICON_FA_FILE_ARCHIVE_O},

    {".fbx", ICON_FA_CODEPEN},
    {".dae", ICON_FA_CODEPEN},
    {".gltf", ICON_FA_CODEPEN},
    {".glb", ICON_FA_CODEPEN},
    {".blend", ICON_FA_CODEPEN},
    {".3ds", ICON_FA_CODEPEN},
    {".ase", ICON_FA_CODEPEN},
    {".obj", ICON_FA_CODEPEN},
    {".ifc", ICON_FA_CODEPEN},
    {".xgl", ICON_FA_CODEPEN},
    {".zgl", ICON_FA_CODEPEN},
    {".ply", ICON_FA_CODEPEN},
    {".dxf", ICON_FA_CODEPEN},
    {".lwo", ICON_FA_CODEPEN},
    {".lws", ICON_FA_CODEPEN},
    {".lxo", ICON_FA_CODEPEN},
    {".stl", ICON_FA_CODEPEN},
    {".x", ICON_FA_CODEPEN},
    {".ac", ICON_FA_CODEPEN},
    {".ms3d", ICON_FA_CODEPEN},
    {".cob", ICON_FA_CODEPEN},
    {".scn", ICON_FA_CODEPEN},
    {".bvh", ICON_FA_CODEPEN},
    {".csm", ICON_FA_CODEPEN},
    {".irrmesh", ICON_FA_CODEPEN},
    {".irr", ICON_FA_CODEPEN},
    {".mdl", ICON_FA_CODEPEN},
    {".md2", ICON_FA_CODEPEN},
    {".md3", ICON_FA_CODEPEN},
    {".pk3", ICON_FA_CODEPEN},
    {".mdc", ICON_FA_CODEPEN},
    {".md5*", ICON_FA_CODEPEN},
    {".smd", ICON_FA_CODEPEN},
    {".vta", ICON_FA_CODEPEN},
    {".ogex", ICON_FA_CODEPEN},
    {".3d", ICON_FA_CODEPEN},
    {".b3d", ICON_FA_CODEPEN},
    {".q3d", ICON_FA_CODEPEN},
    {".q3s", ICON_FA_CODEPEN},
    {".nff", ICON_FA_CODEPEN},
    {".off", ICON_FA_CODEPEN},
    {".raw", ICON_FA_CODEPEN},
    {".ter", ICON_FA_CODEPEN},
    {".hmp", ICON_FA_CODEPEN},
    {".ndo", ICON_FA_CODEPEN},

    {".txt", ICON_FA_FILE_TEXT_O},

    {".cpp", ICON_FA_FILE_CODE_O},
    {".c", ICON_FA_FILE_CODE_O},
    {".cs", ICON_FA_FILE_CODE_O},
    {".md", ICON_FA_FILE_CODE_O},
    {".css", ICON_FA_FILE_CODE_O},
    {".html", ICON_FA_FILE_CODE_O},
    {".js", ICON_FA_FILE_CODE_O},
    {".xml", ICON_FA_FILE_CODE_O},
    {".ini", ICON_FA_FILE_CODE_O},
    {".json", ICON_FA_FILE_CODE_O},
    {".yaml", ICON_FA_FILE_CODE_O},
    {".csv", ICON_FA_FILE_CODE_O},

    {".db", ICON_FA_DATABASE},
};

FileSystem::FileSystem() {
    const auto& args = Engine::Get()->getCommandLineArgs();
    PHYSFS_init(args[0].first.c_str());
    PHYSFS_mount(std::filesystem::current_path().c_str(), nullptr, 1);
}

FileSystem::~FileSystem() {
    PHYSFS_deinit();
}

void FileSystem::update() {

}
