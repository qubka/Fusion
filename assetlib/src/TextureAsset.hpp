#pragma once

#include "AssetLoader.hpp"

namespace fe {
    enum class TextureFormat : uint32_t {
        Unknown = 0,
        RGBA8
    };

    struct PageInfo {
        uint32_t width;
        uint32_t height;
        uint32_t compressedSize;
        uint32_t originalSize;
    };

    struct TextureInfo {
        uint64_t textureSize;
        TextureFormat textureFormat;
        CompressionMode compressionMode;

        std::string originalFile;
        std::vector<PageInfo> pages;
    };

    class TextureAsset {
    public:
        TextureInfo readTextureInfo(const AssetFile& file);
        void unpackTexture(const TextureInfo& info, const std::string& source, size_t sourceSize, char* destination);
        void unpackTexturePage(const TextureInfo& info, int pageIndex, const std::string& source, char* destination);
        AssetFile packTexture(const TextureInfo& info, void* pixels);
    };
}
