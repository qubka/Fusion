#include "texture2d.h"

#include "fusion/core/engine.h"
#include "fusion/assets/asset_registry.h"
#include "fusion/bitmaps/bitmap.h"
#include "fusion/graphics/buffers/buffer.h"
#include "fusion/filesystem/file_format.h"
#include "fusion/filesystem/file_system.h"

#include <gli/gli.hpp>

using namespace fe;

Texture2d::Texture2d(uuids::uuid uuid, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, bool mipmap, bool load)
        : Texture{uuid,
                  filter,
                  addressMode,
                  VK_SAMPLE_COUNT_1_BIT,
                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_COLOR_BIT,
                  VK_IMAGE_VIEW_TYPE_2D,
                  VK_FORMAT_R8G8B8A8_UNORM,
                  1,
                  1,
                  { 0, 0, 1 },
                  anisotropic,
                  mipmap} {
    if (load)
        loadFromFile();
}

Texture2d::Texture2d(const glm::uvec2& extent, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter,
                     VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
        : Texture{filter,
                  addressMode,
                  samples,
                  layout,
                  usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_COLOR_BIT,
                  VK_IMAGE_VIEW_TYPE_2D,
                  format,
                  1,
                  1,
                  { extent.x, extent.y, 1 },
                  anisotropic,
                  mipmap}
    {
}

Texture2d::Texture2d(gsl::span<const uint8_t> pixels, const glm::uvec2& extent, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter,
                     VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
        : Texture{pixels,
                  filter,
                  addressMode,
                  samples,
                  layout,
                  usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_COLOR_BIT,
                  VK_IMAGE_VIEW_TYPE_2D,
                  format,
                  1,
                  1,
                  { extent.x, extent.y, 1 },
                  anisotropic,
                  mipmap}
    {
}

Texture2d::Texture2d(const std::unique_ptr<Bitmap>& bitmap, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter,
                     VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
        : Texture{*bitmap,
                  filter,
                  addressMode,
                  samples,
                  layout,
                  usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_COLOR_BIT,
                  VK_IMAGE_VIEW_TYPE_2D,
                  format,
                  1,
                  1,
                  vku::uvec3_cast(bitmap->getExtent()),
                  anisotropic,
                  mipmap} {
}

void Texture2d::loadFromFile() {
    if (loaded) {
        FE_LOG_DEBUG("Texture2d: '{}' already was loaded", path);
        return;
    }

    auto op = AssetRegistry::Get()->getDatabase()->getValue(uuid);
    if (!op.has_value()) {
        FE_LOG_ERROR("Texture2d: [{}] is not valid", uuid);
        return;
    }

    path = std::move(*op);
    name = path.filename().replace_extension().string();

    fs::path filepath{ Engine::Get()->getApp()->getProjectSettings().projectRoot / path }; // get full path

    // That is fast loading approach
    if (FileFormat::IsTextureStorageFile(filepath)) {
#if FUSION_DEBUG
        auto debugStart = DateTime::Now();
#endif
        gli::texture2d texture;
        FileSystem::ReadBytes(filepath, [&texture](gsl::span<const uint8_t> buffer) {
            texture = gli::texture2d{gli::load(reinterpret_cast<const char*>(buffer.data()), buffer.size())};
        });

        if (texture.empty())
            throw std::runtime_error("Texture is empty");

#if FUSION_DEBUG
        FE_LOG_DEBUG("Texture2d '{}' loaded in {}ms", filepath, (DateTime::Now() - debugStart).asMilliseconds<float>());
#endif

        extent.width = static_cast<uint32_t>(texture.extent().x);
        extent.height = static_cast<uint32_t>(texture.extent().y);
        // arrayLayers = 1
        mipLevels = static_cast<uint32_t>(texture.levels());
        format = vku::convert_format(texture.format());

        if (extent.width == 0 || extent.height == 0)
            throw std::runtime_error("Width or height is empty");

        CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, vku::convert_type(texture.target()));
        CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
        CreateImageView(image, view, viewType, format, aspect, mipLevels, 0, arrayLayers, 0);

        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspect, mipLevels, 0, arrayLayers, 0);
        Buffer bufferStaging{texture.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, texture.data()};
        CopyBufferToImage(bufferStaging, image, extent, arrayLayers, 0);
    } else {
        auto loadBitmap = std::make_unique<Bitmap>(filepath);
        extent =  vku::uvec3_cast(loadBitmap->getExtent());
        format = loadBitmap->getFormat();

        if (extent.width == 0 || extent.height == 0)
            throw std::runtime_error("Width or height is empty");

        mipLevels = mipmap ? GetMipLevels(extent) : 1;

        CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
        CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
        CreateImageView(image, view, viewType, format, aspect, mipLevels, 0, arrayLayers, 0);

        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspect, mipLevels, 0, arrayLayers, 0);
        Buffer bufferStaging{loadBitmap->getLength() * arrayLayers, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, loadBitmap->getData<void>()};
        CopyBufferToImage(bufferStaging, image, extent, arrayLayers, 0);
    }

    if (mipmap) {
        CreateMipmaps(image, extent, format, layout, mipLevels, 0, arrayLayers);
    } else {
        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout, aspect, mipLevels, 0, arrayLayers, 0);
    }

    updateDescriptor();

    loaded = true;
}