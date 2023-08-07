#include "file_format.h"
#include "file_system.h"

#include "fusion/imgui/material_design_icons.h"

using namespace fe;


bool FileFormat::IsTextFile(const fs::path& filepath) {
    std::string extension{ FileSystem::GetExtension(filepath) };

    static const std::set<std::string_view> TEXT_FORMATS = {
            ".txt",
            ".cs",
            ".md",
            ".css",
            ".js",
            ".html",
            ".json",
            ".yaml",
            ".csv",
            ".ini",
            ".xml",
            ".cpp",
            ".hpp",
            ".c",
            ".h",
            ".vert",
            ".frag",
            ".comp",
            ".tesc",
            ".tese",
            ".geom",
            ".lua"
    };

    return TEXT_FORMATS.find(extension) != TEXT_FORMATS.end();
}

bool FileFormat::IsAudioFile(const fs::path& filepath) {
    std::string extension{ FileSystem::GetExtension(filepath) };

    static const std::set<std::string_view> AUDIO_FORMATS = {
            ".ogg",
            ".wav",
            ".mp3"
    };

    return AUDIO_FORMATS.find(extension) != AUDIO_FORMATS.end();
}

bool FileFormat::IsShaderFile(const fs::path& filepath) {
    std::string extension{ FileSystem::GetExtension(filepath) };

    static const std::set<std::string_view> SHADER_FORMATS = {
            ".vert",
            ".frag",
            ".comp",
            ".tesc",
            ".tese",
            ".geom"
    };

    return SHADER_FORMATS.find(extension) != SHADER_FORMATS.end();
}

bool FileFormat::IsSceneFile(const fs::path& filepath) {
    std::string extension{ FileSystem::GetExtension(filepath) };

    if (extension == ".fsn")
        return true;

    return false;
}

bool FileFormat::IsModelFile(const fs::path& filepath) {
    std::string extension{ FileSystem::GetExtension(filepath) };

    // http://assimp.sourceforge.net/main_features_Formats.html
    static const std::set<std::string_view> MODEL_FORMATS = {
            ".fbx",
            ".dae",
            ".gltf",
            ".glb",
            ".obj",
            ".3ds",
            ".ase",
            ".ifc",
            ".xgl",
            ".zgl",
            ".ply",
            ".dxf",
            ".lwo",
            ".lws",
            ".lxo",
            ".stl",
            ".x",
            ".ac",
            ".ms3d",
            ".cob",
            ".scn",
    };

    return MODEL_FORMATS.find(extension) != MODEL_FORMATS.end();
}

bool FileFormat::IsTextureFile(const fs::path& filepath) {
    std::string extension{ FileSystem::GetExtension(filepath) };

    static const std::set<std::string_view> TEXTURE_FORMATS = {
            ".jpeg",
            ".jpg",
            ".png",
            ".bmp",
            ".hdr",
            ".psd",
            ".tga",
            ".gif",
            ".pic",
            ".pgm",
            ".ppm",

            ".ktx",
            ".kmg",
            ".dds"
    };

    return TEXTURE_FORMATS.find(extension) != TEXTURE_FORMATS.end();
}

bool FileFormat::IsTextureStorageFile(const fs::path& filepath) {
    std::string extension{ FileSystem::GetExtension(filepath) };

    static const std::set<std::string_view> TEXTURE_FORMATS = {
            ".ktx",
            ".kmg",
            ".dds"
    };

    return TEXTURE_FORMATS.find(extension) != TEXTURE_FORMATS.end();
}

const char* FileFormat::GetIcon(const fs::path& filepath) {
    std::string extension{ FileSystem::GetExtension(filepath) };

    static std::unordered_map<std::string_view, std::string_view> ICON_FORMATS = {
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

            {".vert", ICON_MDI_FILE_EYE},
            {".frag", ICON_MDI_FILE_EYE},
            {".comp", ICON_MDI_FILE_EYE},
            {".tesc", ICON_MDI_FILE_EYE},
            {".tese", ICON_MDI_FILE_EYE},
            {".geom", ICON_MDI_FILE_EYE},

            {".db", ICON_MDI_DATABASE},
    };

    auto key = ICON_FORMATS.find(extension);
    return key != ICON_FORMATS.end() ? key->second.data() : ICON_MDI_FILE;
}

fs::path FileFormat::GetNextFileName(fs::path filepath) {
    std::string stem{ filepath.stem().string() };
    std::string ext{ filepath.extension().string() };

    for (int i = 1; FileSystem::IsExists(filepath); ++i) {
        std::ostringstream os;
        os << stem << "(" << i << ")" << ext;
        filepath.replace_filename(os.str());
    }
    return filepath;
}
