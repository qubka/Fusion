/*
* Vulkan font loader
*
* Copyright(C) 2021-2022 by Nikita Ushakov
*
* This code is licensed under the MIT license(MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <map>

#include "buffer.hpp"
#include "image.hpp"
#include "context.hpp"

namespace vkx {
    /** @brief glyph data */
    struct Glyph {
        glm::vec2 advance;
        glm::vec2 size;
        glm::vec2 bearing;
        glm::vec2 uv;
    };

    /** @brief font texture */
    struct Font : public Image {
        /**
         * Load a 2D texture
         *
         * @param device Vulkan device to create the texture on
         * @param filename File to load (supports .otf and .ttf)
         * @param width Width of the glyph to create
         * @param height Height of glyph to create
         * @param (Optional) format Vulkan format of the image data stored in the file (defaults to R8G8B8A8_UNORM)
         * @param (Optional) filter Texture filtering for the sampler (defaults to VK_FILTER_LINEAR)
         * @param (Optional) imageUsageFlags Usage flags for the texture's image (defaults to VK_IMAGE_USAGE_SAMPLED_BIT)
         */
        /*void loadFromFile(const vkx::Context& context,
                          const std::string& filename,
                          const vk::Extent2D& size,
                          vk::Format format = vk::Format::eR8G8B8A8Unorm,
                          vk::Filter filter = vk::Filter::eLinear,
                          vk::ImageUsageFlags imageUsageFlags = vk::ImageUsageFlagBits::eSampled);*/

        /**
         * Creates a 2D texture from a buffer
         *
         * @param device Vulkan device to create the texture on
         * @param buffer Buffer containing texture data to upload
         * @param width Width of each glyph to create
         * @param height Height of each glyph to create
         * @param (Optional) format Vulkan format of the image data stored in the file  defaults to R8G8B8A8_UNORM)
         * @param (Optional) filter Texture filtering for the sampler (defaults to VK_FILTER_LINEAR)
         * @param (Optional) imageUsageFlags Usage flags for the texture's image (defaults to VK_IMAGE_USAGE_SAMPLED_BIT)
         */
        void fromBuffer(const vkx::Context& context,
                        std::vector<uint8_t>& buffer,
                        const vk::Extent2D& size,
                        vk::Format format = vk::Format::eR8G8B8A8Unorm,
                        vk::Filter filter = vk::Filter::eLinear,
                        vk::ImageUsageFlags imageUsageFlags = vk::ImageUsageFlagBits::eSampled) {
            device = context.device;

            // Create target image for copy
            vk::ImageCreateInfo imageCreateInfo;
            imageCreateInfo.imageType = vk::ImageType::e2D;
            imageCreateInfo.format = format;
            imageCreateInfo.extent.width = size.width;
            imageCreateInfo.extent.height = size.height;
            imageCreateInfo.extent.depth = 1;
            imageCreateInfo.mipLevels = 1;
            imageCreateInfo.arrayLayers = 1;
            // Ensure that the TRANSFER_DST bit is set for staging
            imageCreateInfo.usage = imageUsageFlags | vk::ImageUsageFlagBits::eTransferDst;

            static_cast<vkx::Image&>(*this) = context.stageToDeviceImage(imageCreateInfo, buffer);

            // Image view
            vk::ImageViewCreateInfo viewCreateInfo;
            viewCreateInfo.image = image;
            viewCreateInfo.viewType = vk::ImageViewType::e2D;
            viewCreateInfo.format = format;
            viewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            viewCreateInfo.subresourceRange.levelCount = 1;
            viewCreateInfo.subresourceRange.layerCount = 1;
            view = device.createImageView(viewCreateInfo);

            // Font texture Sampler
            vk::SamplerCreateInfo samplerCreateInfo;
            samplerCreateInfo.magFilter = filter;
            samplerCreateInfo.minFilter = filter;
            samplerCreateInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
            samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
            samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
            samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
            samplerCreateInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;
            sampler = device.createSampler(samplerCreateInfo);
        }
    };

    /** @brief font atlas texture with cache */
    struct FontAtlas : public Image {
        std::map<wchar_t, Glyph> glyphs;

        /**
         * Load a 2D texture as atlas
         *
         * @param device Vulkan device to create the font on
         * @param filename File to load (supports .otf and .ttf)
         * @param width Width of the glyph to create
         * @param height Height of the glyph to create
         * @param (Optional) format Vulkan format of the image data stored in the file (defaults to R8G8B8A8_UNORM)
         * @param (Optional) filter Texture filtering for the sampler (defaults to VK_FILTER_LINEAR)
         * @param (Optional) imageUsageFlags Usage flags for the texture's image (defaults to VK_IMAGE_USAGE_SAMPLED_BIT)
         */
        void loadFromFile(const vkx::Context& context,
                          const std::string& filename,
                          const vk::Extent2D& size,
                          vk::Format format = vk::Format::eR8G8B8A8Unorm,
                          vk::Filter filter = vk::Filter::eLinear,
                          vk::ImageUsageFlags imageUsageFlags = vk::ImageUsageFlagBits::eSampled);
    };
} // namespace vkx