#pragma once

namespace Fusion {
    struct FUSION_API AssetFile {
        char type[4];
        int version;
        std::string serialized;
        std::vector<char> binaryBlob;
    };

    enum class CompressionMode : uint32_t {
        None,
        LZ4
    };

    class FUSION_API Asset {
    public:
        static bool SaveBinaryFile(const std::string& path, const AssetFile& file);
        static bool LoadBinaryFile(const std::string& path, AssetFile& file);
        static CompressionMode parseCompression(const char* f);
    };
}
