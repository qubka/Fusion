#include "files.hpp"

using namespace fe;

std::vector<std::filesystem::directory_entry> fs::walk(const std::filesystem::path& dir) {
    std::vector<std::filesystem::directory_entry> files;

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        files.push_back(entry);
    }

    std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
        return a.is_directory() && !b.is_directory();
    });

    return files;
}

const char* fs::ICON_FA_FOLDER_CLOSE{ "\uF07B" };
const char* fs::ICON_FA_FOLDER_OPEN{ "\uF07C" };
const char* fs::ICON_FA_FILE{ "\uF016" };
const char* fs::ICON_FA_SEARCH{ "\uF002" };
const char* fs::ICON_FA_REPLY{ "\uF112" };
const char* fs::ICON_FA_PLAY{ "\uF04B" };
const char* fs::ICON_FA_STOP{ "\uF04D" };
const char* fs::ICON_FA_OK{ "\uF00C" };
const char* fs::ICON_FA_REMOVE{ "\uF00D" };
const char* fs::ICON_FA_AXIS{ "\uF1E9" };
const char* fs::ICON_FA_CAMERA{ "\uF030" };
const char* fs::ICON_FA_CUBE{ "\uF1B2" };
const char* fs::ICON_FA_CUBES{ "\uF1B3" };
const char* fs::ICON_FA_MANY_CUBES{ "\uF2B8" };
const char* fs::ICON_FA_INFO{ "\uF05A" };
const char* fs::ICON_FA_LIST{ "\uF03A" };
const char* fs::ICON_FA_STATS{ "\uF1EA" };
const char* fs::ICON_FA_ARCHIVE{ "\uF187" };
const char* fs::ICON_FA_STORAGE{ "\uF1C0" };

std::unordered_map<std::string, std::string> fs::extensions {
    {".gif", "\uF1C5"},
    {".jpeg", "\uF1C5"},
    {".jpg", "\uF1C5"},
    {".png", "\uF1C5"},
    {".ktx", "\uF1C5"},

    {".pdf", "\uF1C1"},

    {".doc", "\uF1C2"},
    {".docx", "\uF1C2"},

    {".ppt", "\uF1C4"},
    {".pptx", "\uF1C4"},

    {".xls", "\uF1C3"},
    {".xlsx", "\uF1C3"},

    {".csv", "\uF1C0"},
    {".db", "\uF1C0"},

    {".aac", "\uF1C7"},
    {".mp3", "\uF1C7"},
    {".ogg", "\uF1C7"},

    {".avi", "\uF1C8"},
    {".flv", "\uF1C8"},
    {".mkv", "\uF1C8"},
    {".mp4", "\uF1C8"},

    {".gz", "\uF1C6"},
    {".zip", "\uF1C6"},
    {".7z", "\uF1C6"},
    {".tar", "\uF1C6"},
    {".tar.bz2", "\uF1C6"},
    {".tar.Z", "\uF1C6"},
    {".tar.gz", "\uF1C6"},
    {".tar.lz4", "\uF1C6"},
    {".tar.lz", "\uF1C6"},
    {".tar.lzma", "\uF1C6"},
    {".tar.xz", "\uF1C6"},
    {".tar.zst", "\uF1C6"},

    {".css", "\uF1C9"},
    {".html", "\uF1C9"},
    {".js", "\uF1C9"},
    {".xml", "\uF1C9"},

    {".yaml", "\uF1D4"},

    {".fbx", "\uF1CB"},
    {".dae", "\uF1CB"},
    {".gltf", "\uF1CB"},
    {".glb", "\uF1CB"},
    {".blend", "\uF1CB"},
    {".3ds", "\uF1CB"},
    {".ase", "\uF1CB"},
    {".obj", "\uF1CB"},
    {".ifc", "\uF1CB"},
    {".xgl", "\uF1CB"},
    {".zgl", "\uF1CB"},
    {".ply", "\uF1CB"},
    {".dxf", "\uF1CB"},
    {".lwo", "\uF1CB"},
    {".lws", "\uF1CB"},
    {".lxo", "\uF1CB"},
    {".stl", "\uF1CB"},
    {".x", "\uF1CB"},
    {".ac", "\uF1CB"},
    {".ms3d", "\uF1CB"},
    {".cob", "\uF1CB"},
    {".scn", "\uF1CB"},
    {".bvh", "\uF1CB"},
    {".csm", "\uF1CB"},
    {".irrmesh", "\uF1CB"},
    {".irr", "\uF1CB"},
    {".mdl", "\uF1CB"},
    {".md2", "\uF1CB"},
    {".md3", "\uF1CB"},
    {".pk3", "\uF1CB"},
    {".mdc", "\uF1CB"},
    {".md5*", "\uF1CB"},
    {".smd", "\uF1CB"},
    {".vta", "\uF1CB"},
    {".ogex", "\uF1CB"},
    {".3d", "\uF1CB"},
    {".b3d", "\uF1CB"},
    {".q3d", "\uF1CB"},
    {".q3s", "\uF1CB"},
    {".nff", "\uF1CB"},
    {".off", "\uF1CB"},
    {".raw", "\uF1CB"},
    {".ter", "\uF1CB"},
    {".hmp", "\uF1CB"},
    {".ndo", "\uF1CB"},

    {".txt", "\uF016"},
    {".md", "\uF016"}
};

std::string fs::extension_icon(const std::filesystem::path& file) {
    auto key { fs::extensions.find(file.extension()) };
    return key != fs::extensions.end() ? key->second.c_str() : fs::ICON_FA_FILE;
}
