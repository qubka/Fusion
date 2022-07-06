#include "file_system.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/utils/string.hpp"
#include "fusion/imgui/material_design_icons.hpp"

#include <physfs.h>

using namespace fe;

bool FileSystem::Exists(const std::filesystem::path& path) {
    return PHYSFS_exists(path.c_str()) != 0;
}

void FileSystem::Read(const std::filesystem::path& filename, const FileSystem::SimpleHandler& handler) {
    auto bytes = ReadBytes(filename);
    handler(bytes.data(), bytes.size());
}

std::vector<uint8_t> FileSystem::ReadBytes(const std::filesystem::path& filename) {
    auto fsFile = PHYSFS_openRead(filename.c_str());

    if (!fsFile) {
        if (!std::filesystem::exists(filename) || !std::filesystem::is_regular_file(filename)) {
            LOG_ERROR << "Failed to open file " << filename << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
            return {};
        }

        std::ifstream is{filename, std::ios::binary};
        is.unsetf(std::ios::skipws);

        std::streampos filesize;

        is.seekg(0, std::ios::end);
        filesize = is.tellg();
        is.seekg(0, std::ios::beg);

        std::vector<uint8_t> filedata;
        filedata.reserve(filesize);

        filedata.insert(filedata.begin(), std::istream_iterator<uint8_t>(is), std::istream_iterator<uint8_t>());

        return filedata;
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
    auto fsFile = PHYSFS_openRead(filename.c_str());

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

bool FileSystem::WriteFile(const std::filesystem::path& filename, uint8_t* buffer, size_t size) {
    auto fsFile = PHYSFS_openWrite(filename.c_str());

    if (!fsFile) {
        if (!std::filesystem::exists(filename) || !std::filesystem::is_regular_file(filename)) {
            LOG_ERROR << "Failed to open file " << filename << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
            return false;
        }

        std::ofstream os{filename, std::ios::binary};
        os.write(reinterpret_cast<const char*>(buffer), size);
        return true;
    }

    bool ret = false;

    auto written = PHYSFS_writeBytes(fsFile, reinterpret_cast<const void*>(buffer), size);
    if(written != size)
        LOG_ERROR << "Failed to write in file " << filename << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    else
        ret = written;

    if (PHYSFS_close(fsFile) == 0) {
        LOG_ERROR << "Failed to close file " << filename << ", " << PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
    }

    return ret;
}

std::vector<std::filesystem::path> FileSystem::GetFiles(const std::filesystem::path& path, bool recursive) {
    auto rc = PHYSFS_enumerateFiles(path.c_str());

    std::vector<std::filesystem::path> files;

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

bool FileSystem::ContainsDirectories(const std::filesystem::path& path) {
    bool ret = false;
    if (IsDirectory(path)) {
        auto rc = PHYSFS_enumerateFiles(path.c_str());

        for (auto i = rc; *i; i++) {
            if (IsDirectory(*i)) {
                ret = true;
                break;
            }
        }

        PHYSFS_freeList(rc);
    }
    return ret;
}

bool FileSystem::IsDirectory(const std::filesystem::path& path) {
    PHYSFS_Stat stat;
    if (!PHYSFS_stat(path.c_str(), &stat)) {
        return false;
    } else {
        if (stat.filetype == PHYSFS_FILETYPE_SYMLINK) {
            // PHYSFS_stat() doesn't follow symlinks, so we do it manually
            const char* realdir = PHYSFS_getRealDir(path.c_str());
            if (realdir == nullptr) {
                return false;
            } else {
                const std::filesystem::path realfile = std::filesystem::path{realdir} / path;
                return IsDirectory(realfile);
            }
        } else {
            return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
        }
    }
}

FileStats FileSystem::GetStats(const std::filesystem::path& path) {
    PHYSFS_Stat stat = {};
    PHYSFS_stat(path.c_str(), &stat);

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

    PHYSFS_Stat stat = {};
    PHYSFS_stat(path.c_str(), &stat);

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

std::string FileSystem::GetExtension(const std::filesystem::path& path) {
    return String::Lowercase(path.extension().string());
}

const char* FileSystem::GetIcon(const std::filesystem::path& path) {
    auto key = Extensions.find(GetExtension(path));
    return key != Extensions.end() ? key->second.c_str() : ICON_MDI_FILE;
}

std::unordered_map<std::string, std::string> FileSystem::Extensions = {
    {".gif", ICON_MDI_FILE_IMAGE},
    {".jpeg", ICON_MDI_FILE_IMAGE},
    {".jpg", ICON_MDI_FILE_IMAGE},
    {".png", ICON_MDI_FILE_IMAGE},
    {".ktx", ICON_MDI_FILE_IMAGE},

    {".pdf", ICON_MDI_FILE_PDF},

    {".doc", ICON_MDI_FILE_WORD},
    {".docx", ICON_MDI_FILE_WORD},

    {".ppt", ICON_MDI_FILE_POWERPOINT},
    {".pptx", ICON_MDI_FILE_POWERPOINT},

    {".xls", ICON_MDI_FILE_EXCEL},
    {".xlsx", ICON_MDI_FILE_EXCEL},

    {".aac", ICON_MDI_FILE_MUSIC},
    {".mp3", ICON_MDI_FILE_MUSIC},
    {".ogg", ICON_MDI_FILE_MUSIC},

    {".avi", ICON_MDI_FILE_VIDEO},
    {".flv", ICON_MDI_FILE_VIDEO},
    {".mkv", ICON_MDI_FILE_VIDEO},
    {".mp4", ICON_MDI_FILE_VIDEO},

    {".gz", ICON_MDI_ARCHIVE},
    {".zip", ICON_MDI_ARCHIVE},
    {".7z", ICON_MDI_ARCHIVE},
    {".tar", ICON_MDI_ARCHIVE},
    {".tar.bz2", ICON_MDI_ARCHIVE},
    {".tar.Z", ICON_MDI_ARCHIVE},
    {".tar.gz", ICON_MDI_ARCHIVE},
    {".tar.lz4", ICON_MDI_ARCHIVE},
    {".tar.lz", ICON_MDI_ARCHIVE},
    {".tar.lzma", ICON_MDI_ARCHIVE},
    {".tar.xz", ICON_MDI_ARCHIVE},
    {".tar.zst", ICON_MDI_ARCHIVE},

    {".fbx", ICON_MDI_CODEPEN},
    {".dae", ICON_MDI_CODEPEN},
    {".gltf", ICON_MDI_CODEPEN},
    {".glb", ICON_MDI_CODEPEN},
    {".blend", ICON_MDI_CODEPEN},
    {".3ds", ICON_MDI_CODEPEN},
    {".ase", ICON_MDI_CODEPEN},
    {".obj", ICON_MDI_CODEPEN},
    {".ifc", ICON_MDI_CODEPEN},
    {".xgl", ICON_MDI_CODEPEN},
    {".zgl", ICON_MDI_CODEPEN},
    {".ply", ICON_MDI_CODEPEN},
    {".dxf", ICON_MDI_CODEPEN},
    {".lwo", ICON_MDI_CODEPEN},
    {".lws", ICON_MDI_CODEPEN},
    {".lxo", ICON_MDI_CODEPEN},
    {".stl", ICON_MDI_CODEPEN},
    {".x", ICON_MDI_CODEPEN},
    {".ac", ICON_MDI_CODEPEN},
    {".ms3d", ICON_MDI_CODEPEN},
    {".cob", ICON_MDI_CODEPEN},
    {".scn", ICON_MDI_CODEPEN},
    {".bvh", ICON_MDI_CODEPEN},
    {".csm", ICON_MDI_CODEPEN},
    {".irrmesh", ICON_MDI_CODEPEN},
    {".irr", ICON_MDI_CODEPEN},
    {".mdl", ICON_MDI_CODEPEN},
    {".md2", ICON_MDI_CODEPEN},
    {".md3", ICON_MDI_CODEPEN},
    {".pk3", ICON_MDI_CODEPEN},
    {".mdc", ICON_MDI_CODEPEN},
    {".md5*", ICON_MDI_CODEPEN},
    {".smd", ICON_MDI_CODEPEN},
    {".vta", ICON_MDI_CODEPEN},
    {".ogex", ICON_MDI_CODEPEN},
    {".3d", ICON_MDI_CODEPEN},
    {".b3d", ICON_MDI_CODEPEN},
    {".q3d", ICON_MDI_CODEPEN},
    {".q3s", ICON_MDI_CODEPEN},
    {".nff", ICON_MDI_CODEPEN},
    {".off", ICON_MDI_CODEPEN},
    {".raw", ICON_MDI_CODEPEN},
    {".ter", ICON_MDI_CODEPEN},
    {".hmp", ICON_MDI_CODEPEN},
    {".ndo", ICON_MDI_CODEPEN},

    {".txt", ICON_MDI_TEXTBOX},

    {".cpp", ICON_MDI_FILE_XML},
    {".c", ICON_MDI_FILE_XML},
    {".cs", ICON_MDI_FILE_XML},
    {".md", ICON_MDI_FILE_XML},
    {".css", ICON_MDI_FILE_XML},
    {".html", ICON_MDI_FILE_XML},
    {".js", ICON_MDI_FILE_XML},
    {".xml", ICON_MDI_FILE_XML},
    {".ini", ICON_MDI_FILE_XML},
    {".json", ICON_MDI_FILE_XML},
    {".yaml", ICON_MDI_FILE_XML},
    {".csv", ICON_MDI_FILE_XML},

    {".db", ICON_MDI_DATABASE},
};

FileSystem::FileSystem() {
    // TODO: Make
    const auto& args = Engine::Get()->getCommandLineArgs();
    PHYSFS_init(args[0].first.c_str());
    //PHYSFS_mount("/", nullptr, 1);
}

FileSystem::~FileSystem() {
    PHYSFS_deinit();
}

void FileSystem::onUpdate() {

}