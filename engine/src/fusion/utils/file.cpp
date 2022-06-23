#include "file.hpp"
#include "storage.hpp"

#include <IconsFontAwesome4.h>

using namespace fe;

void File::WithBinaryFileContents(const std::filesystem::path& filename, const File::SimpleHandler& handler) {
    auto storage = Storage::ReadFile(filename);
    handler(storage->size(), storage->data());
}

std::vector<uint8_t> File::ReadBinaryFile(const std::filesystem::path& filename) {
    std::vector<uint8_t> result;
    WithBinaryFileContents(filename, [&](size_t size, const void* data) {
        result.resize(size);
        memcpy(result.data(), data, size);
    });
    return result;
}

std::string File::ReadTextFile(const std::filesystem::path& filename) {
    std::string content;
    std::ifstream infile{filename, std::ios::in};
    if (!infile.is_open()) {
        throw std::runtime_error("File " + filename.string() + " not found");
    }
    std::string line;
    while (!infile.eof()) {
        getline(infile, line);
        content.append(line + "\n");
    }
    infile.close();
    return content;
}

std::string File::ExtensionIcon(const std::filesystem::path& filename) {
    auto key = Extensions.find(filename.extension().string());
    return key != Extensions.end() ? key->second.c_str() : ICON_FA_FILE;
}

std::map<std::string, std::string> File::Extensions = {
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