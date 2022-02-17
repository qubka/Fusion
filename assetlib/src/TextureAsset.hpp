#pragma once

#include "AssetLoader.hpp"

namespace Fusion {
    enum class TextureFormat : uint32_t {
        Unknown = 0,
        RGBA8
    };

    struct FUSION_API PageInfo {
        uint32_t width;
        uint32_t height;
        uint32_t compressedSize;
        uint32_t originalSize;
    };

    struct FUSION_API TextureInfo {
        uint64_t textureSize;
        TextureFormat textureFormat;
        CompressionMode compressionMode;

        std::string originalFile;
        std::vector<PageInfo> pages;
    };

    class FUSION_API TextureAsset {
    public:
        TextureInfo readTextureInfo(const AssetFile& file);
        void unpackTexture(const TextureInfo& info, const std::string& source, size_t sourceSize, char* destination);
        void unpackTexturePage(const TextureInfo& info, int pageIndex, const std::string& source, char* destination);
        AssetFile packTexture(const TextureInfo& info, void* pixels);
    };
}
