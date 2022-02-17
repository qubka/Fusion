#include "TextureAsset.hpp"
#include <json.hpp>
#include <lz4.h>

using namespace Fusion;

TextureInfo TextureAsset::readTextureInfo(const AssetFile& file) {
    TextureInfo info;

    nlohmann::json texture_metadata = nlohmann::json::parse(file.serialized);

    std::string formatString = texture_metadata["format"];
    info.textureFormat = parse_format(formatString.c_str());

    std::string compressionString = texture_metadata["compression"];
    info.compressionMode = parse_compression(compressionString.c_str());

    info.textureSize = texture_metadata["buffer_size"];
    info.originalFile = texture_metadata["original_file"];

    for (auto& [key, value] : texture_metadata["pages"].items()) {
        PageInfo page;

        page.compressedSize = value["compressed_size"];
        page.originalSize = value["original_size"];
        page.width = value["width"];
        page.height = value["height"];

        info.pages.push_back(page);
    }


    return info;
}

void TextureAsset::unpackTexture(const TextureInfo& info, const std::string& source, size_t sourceSize, char* destination) {

}

void TextureAsset::unpackTexturePage(const TextureInfo& info, int pageIndex, const std::string& source, char* destination) {

}

AssetFile TextureAsset::packTexture(const TextureInfo& info, void* pixels) {
    return AssetFile();
}
