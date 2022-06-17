/*
* Vulkan Font loader using FreeFont
*
* Copyright(C) 2021-2022 by Nikita Ushakov
*
* This code is licensed under the MIT license(MIT) (http://opensource.org/licenses/MIT)
*/

#include "font.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

using namespace vkx;

FT_UInt countChars(const FT_Face& face) {
    FT_UInt index;
    FT_ULong character = FT_Get_First_Char(face, &index);

    while (index != 0) {
        character = FT_Get_Next_Char(face, character, &index);
    }

    return index;
}

void FontAtlas::fromFile(const vkx::Context& context, const std::string& filename, const vk::Extent2D& size, vk::Format format, vk::Filter filter, vk::ImageUsageFlags imageUsageFlags) {
    assert(std::filesystem::exists(filename));

    device = context.device;

    // TODO: make be move ft to wrapper?
    FT_Library library;
    if (FT_Init_FreeType(&library)) {
        throw std::runtime_error("Failed to init FreeType!");
    }

    FT_Face face;
    if (FT_New_Face(library, filename.c_str(), 0, &face)) {
        throw std::runtime_error("Failed to load font: " + filename);
    }

    FT_Set_Pixel_Sizes(face, size.width, size.height);

    // https://gist.github.com/baines/b0f9e4be04ba4e6f56cab82eef5008ff

    int height = 1 + (face->size->metrics.height >> 6);
    glm::ivec2 o{0, 0};
    int maxDim = height * std::ceil(std::sqrt(countChars(face) / 2));

    int width = 1;
    while (width < maxDim) width <<= 1;

    std::vector<uint8_t> buffer(width * width);

    const auto& glyph = face->glyph;

    FT_UInt index;
    FT_ULong character = FT_Get_First_Char(face, &index);
    while (index != 0) {
        const FT_Bitmap* bmp = &glyph->bitmap;

        if (o.x + bmp->width >= width) {
            o.x = 0;
            o.y += height;
        }

        for (int row = 0; row < bmp->rows; ++row) {
            for (int col = 0; col < bmp->width; ++col) {
                int x = o.x + col;
                int y = o.y + row;
                buffer[y * width + x] = bmp->buffer[row * bmp->pitch + col];
            }
        }

        glyphs.emplace(character, Glyph{
            {glyph->advance.x >> 6, glyph->advance.y >> 6},
            {glyph->bitmap.width,   glyph->bitmap.rows},
            {glyph->bitmap_left,    glyph->bitmap_top},
            {static_cast<float>(o.x) / static_cast<float>(width), static_cast<float>(o.y) / static_cast<float>(width)}
        });

        o.x += bmp->width + 1;

        character = FT_Get_Next_Char(face, character, &index);
    }

    // Create optimal tiled target image
    vk::ImageCreateInfo imageCreateInfo;
    imageCreateInfo.imageType = vk::ImageType::e2D;
    imageCreateInfo.format = format;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.extent.width = size.width;
    imageCreateInfo.extent.height = size.height;
    imageCreateInfo.extent.depth = 1;
    // Ensure that the TRANSFER_DST bit is set for staging
    imageCreateInfo.usage = imageUsageFlags | vk::ImageUsageFlagBits::eTransferDst;

    static_cast<vkx::Image&>(*this) = context.stageToDeviceImage(imageCreateInfo, buffer);

    // Create image view
    vk::ImageViewCreateInfo viewCreateInfo;
    viewCreateInfo.image = image;
    viewCreateInfo.viewType = vk::ImageViewType::e2D;
    viewCreateInfo.format = format;
    viewCreateInfo.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
    view = device.createImageView(viewCreateInfo);

    // Create sampler
    vk::SamplerCreateInfo samplerCreateInfo;
    samplerCreateInfo.magFilter = filter;
    samplerCreateInfo.minFilter = filter;
    samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    samplerCreateInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;
    samplerCreateInfo.maxAnisotropy = context.deviceFeatures.samplerAnisotropy ? context.deviceProperties.limits.maxSamplerAnisotropy : 1.0f;
    sampler = device.createSampler(samplerCreateInfo);

    FT_Done_Face(face);
    FT_Done_FreeType(library);
}
