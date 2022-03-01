#pragma once

namespace fe {
    class ContentBrowserPanel {
    public:
        ContentBrowserPanel();
        ~ContentBrowserPanel() = default;

        void onImGui();

        const std::filesystem::path& getCurrentFile() const { return currentFile; }
        const std::filesystem::path& getCurrentDirectory() const { return currentDirectory; }

    private:
        std::filesystem::path currentDirectory;
        std::filesystem::path currentFile;

        std::unordered_map<std::string, std::string> extentions = {
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
    };
}
