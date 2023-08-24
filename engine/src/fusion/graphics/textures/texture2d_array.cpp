#include "texture2d_array.h"

#include "fusion/core/engine.h"
#include "fusion/assets/asset_registry.h"
#include "fusion/bitmaps/bitmap.h"
#include "fusion/graphics/buffers/buffer.h"
#include "fusion/graphics/commands/command_buffer.h"
#include "fusion/filesystem/file_format.h"
#include "fusion/filesystem/file_system.h"

#include <gli/gli.hpp>

using namespace fe;

Texture2dArray::Texture2dArray(uuids::uuid uuid, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, bool mipmap, bool load)
        : Texture{uuid,
                  filter,
                  addressMode,
                  VK_SAMPLE_COUNT_1_BIT,
                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_COLOR_BIT,
                  VK_IMAGE_VIEW_TYPE_2D_ARRAY,
                  VK_FORMAT_R8G8B8A8_UNORM,
                  1,
                  0,
                  { 0, 0, 1 },
                  anisotropic,
                  mipmap} {
    if (load)
        loadFromFile();
}

Texture2dArray::Texture2dArray(const glm::uvec2& extent, uint32_t arrayLayers, VkFormat format, VkImageLayout layout,
                               VkImageUsageFlags usage, VkFilter filter, VkSamplerAddressMode addressMode,
                               bool anisotropic, bool mipmap)
        : Texture{filter,
                  addressMode,
                  VK_SAMPLE_COUNT_1_BIT,
                  layout,
                  usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_COLOR_BIT,
                  VK_IMAGE_VIEW_TYPE_2D_ARRAY,
                  format,
                  1,
                  arrayLayers,
                  { extent.x, extent.y, 1 },
                  anisotropic,
                  mipmap} {
}

Texture2dArray::Texture2dArray(gsl::span<const uint8_t> pixels, const glm::uvec2& extent, uint32_t arrayLayers, VkFormat format, VkImageLayout layout,
                               VkImageUsageFlags usage, VkFilter filter, VkSamplerAddressMode addressMode,
                               bool anisotropic, bool mipmap)
        : Texture{pixels,
                  filter,
                  addressMode,
                  VK_SAMPLE_COUNT_1_BIT,
                  layout,
                  usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_COLOR_BIT,
                  VK_IMAGE_VIEW_TYPE_2D_ARRAY,
                  format,
                  1,
                  arrayLayers,
                  { extent.x, extent.y, 1 },
                  anisotropic,
                  mipmap} {
}

Texture2dArray::Texture2dArray(const std::unique_ptr<Bitmap>& bitmap, uint32_t arrayLayers, VkFormat format,
                               VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter,
                               VkSamplerAddressMode addressMode, bool anisotropic, bool mipmap)
        : Texture{*bitmap,
                  filter,
                  addressMode,
                  VK_SAMPLE_COUNT_1_BIT,
                  layout,
                  usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_COLOR_BIT,
                  VK_IMAGE_VIEW_TYPE_2D_ARRAY,
                  format,
                  1,
                  arrayLayers,
                  vku::uvec3_cast(bitmap->getExtent()),
                  anisotropic,
                  mipmap} {
}

void Texture2dArray::loadFromFile() {
    if (loaded) {
        FE_LOG_DEBUG("Texture2dArray: '{}' already was loaded", path);
        return;
    }

    auto op = AssetRegistry::Get()->getDatabase()->getValue(uuid);
    if (!op.has_value()) {
        FE_LOG_ERROR("Texture2dArray: [{}] is not valid", uuid);
        return;
    }

    if (!FileFormat::IsTextureStorageFile(*op)) {
        FE_LOG_ERROR("Texture2dArray: Unsupported format for fast and single loading");
        return;
    }

    path = std::move(*op);
    name = path.filename().replace_extension().string();

    fs::path filepath{ Engine::Get()->getApp()->getProjectSettings().projectRoot / path }; // get full path

#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif
    gli::texture2d_array texture;
    FileSystem::ReadBytes(filepath, [&texture](gsl::span<const uint8_t> buffer) {
        texture = gli::texture2d_array{gli::load(reinterpret_cast<const char*>(buffer.data()), buffer.size())};
    });

    if (texture.empty())
        throw std::runtime_error("Texture is empty");

#if FUSION_DEBUG
    FE_LOG_DEBUG("Texture2dArray: '{}' loaded in {}ms", filepath, (DateTime::Now() - debugStart).asMilliseconds<float>());
#endif

    extent.width = static_cast<uint32_t>(texture.extent().x);
    extent.height = static_cast<uint32_t>(texture.extent().y);
    arrayLayers = static_cast<uint32_t>(texture.layers());
    mipLevels = static_cast<uint32_t>(texture.levels());
    format = vku::convert_format(texture.format());

    if (extent.width == 0 || extent.height == 0)
        throw std::runtime_error("Width or height is empty");

    CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, vku::convert_type(texture.target()));
    CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
    CreateImageView(image, view, viewType, format, aspect, mipLevels, 0, arrayLayers, 0);

    TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspect, mipLevels, 0, arrayLayers, 0);
    Buffer bufferStaging{texture.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, texture.data()};

    // Setup buffer copy regions for each layer including all of it's miplevels
    std::vector<VkBufferImageCopy> bufferCopyRegions;
    bufferCopyRegions.reserve(arrayLayers * mipLevels);
    VkDeviceSize offset = 0;
    for (uint32_t layer = 0; layer < arrayLayers; ++layer) {
        for (uint32_t level = 0; level < mipLevels; ++level) {
            auto image = texture[layer][level];
            auto imageExtent = image.extent();

            auto& region = bufferCopyRegions.emplace_back();
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = aspect;
            region.imageSubresource.mipLevel = level;
            region.imageSubresource.baseArrayLayer = layer;
            region.imageSubresource.layerCount = 1;
            region.imageOffset = {0, 0, 0};
            region.imageExtent.width = static_cast<uint32_t>(imageExtent.x);
            region.imageExtent.height = static_cast<uint32_t>(imageExtent.y);
            region.imageExtent.depth = 1;
            region.bufferOffset = offset;

            // Increase offset into staging buffer for next level / face
            offset += image.size();
        }
    }

    CommandBuffer commandBuffer{true};
    vkCmdCopyBufferToImage(commandBuffer, bufferStaging, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());
    commandBuffer.submitIdle();

    if (mipmap) {
        CreateMipmaps(image, extent, format, layout, mipLevels, 0, arrayLayers);
    } else {
        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout, aspect, mipLevels, 0, arrayLayers, 0);
    }

    updateDescriptor();

    loaded = true;
}
