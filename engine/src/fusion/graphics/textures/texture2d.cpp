#include "texture2d.hpp"

#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/graphics/vku.hpp"
#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/filesystem/file_format.hpp"
#include "fusion/filesystem/file_system.hpp"

#include <gli/gli.hpp>

using namespace fe;

Texture2d::Texture2d(fs::path filepath, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, bool mipmap, bool load)
        : Texture{std::move(filepath),
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
                  0,
                  anisotropic,
                  mipmap} {
    if (load) {
        Texture2d::load();
    }
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
                  vku::getBlockParams(format).bytes,
                  anisotropic,
                  mipmap}
    {
}

Texture2d::Texture2d(const std::unique_ptr<Bitmap>& bitmap, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter,
                     VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
        : Texture{bitmap,
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
                  bitmap->getComponents(),
                  anisotropic,
                  mipmap} {
}

void Texture2d::setPixels(const uint8_t* pixels, uint32_t layerCount, uint32_t baseArrayLayer) {
    CommandBuffer commandBuffer{true};
    Buffer bufferStaging{extent.width * extent.height * components * arrayLayers, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pixels};
    CopyBufferToImage(commandBuffer, bufferStaging, image, extent, layerCount, baseArrayLayer);
    commandBuffer.submitIdle();
}

void Texture2d::load(std::unique_ptr<Bitmap> loadBitmap) {
    CommandBuffer commandBuffer{true};

    bool loadFromFile = !path.empty() && !loadBitmap;
    // That is fast loading approach
    if (loadFromFile && FileFormat::IsTextureStorageFile(path)) {
#if FUSION_DEBUG
        auto debugStart = DateTime::Now();
#endif
        std::unique_ptr<gli::texture2d> texture;
        FileSystem::Read(path, [&texture](const uint8_t* data, size_t size) {
            texture = std::make_unique<gli::texture2d>(gli::load(reinterpret_cast<const char*>(data), size));
        });
#if FUSION_DEBUG
        LOG_DEBUG << "Texture2d \"" << path << "\" loaded in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
        const gli::texture2d& tex = *texture;
        if (tex.empty())
            throw std::runtime_error("Texture is empty");

        extent.width = static_cast<uint32_t>(tex.extent().x);
        extent.height = static_cast<uint32_t>(tex.extent().y);
        // arrayLayers = 1
        mipLevels = static_cast<uint32_t>(tex.levels());
        components = static_cast<uint8_t>(component_count(tex.format()));

        if (extent.width == 0 || extent.height == 0)
            throw std::runtime_error("Width or height is empty");

        CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
        CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
        CreateImageView(image, view, viewType, format, aspect, mipLevels, 0, arrayLayers, 0);
        TransitionImageLayout(commandBuffer, image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspect, mipLevels, 0, arrayLayers, 0);

        Buffer bufferStaging{tex.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, tex.data()};
        CopyBufferToImage(commandBuffer, bufferStaging, image, extent, arrayLayers, 0);

        if (mipmap) {
            CreateMipmaps(commandBuffer, image, extent, format, layout, mipLevels, 0, arrayLayers);
        } else {
            TransitionImageLayout(commandBuffer, image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout, aspect, mipLevels, 0, arrayLayers, 0);
        }
    } else {
        if (loadFromFile) {
            loadBitmap = std::make_unique<Bitmap>(path);
            extent =  vku::uvec3_cast(loadBitmap->getExtent());
            components = loadBitmap->getComponents();
        }

        if (extent.width == 0 || extent.height == 0)
            throw std::runtime_error("Width or height is empty");

        mipLevels = mipmap ? GetMipLevels(extent) : 1;

        CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
        CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
        CreateImageView(image, view, viewType, format, aspect, mipLevels, 0, arrayLayers, 0);
        TransitionImageLayout(commandBuffer, image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspect, mipLevels, 0, arrayLayers, 0);

        Buffer bufferStaging{loadBitmap->getLength(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, loadBitmap->getData<void>()};
        CopyBufferToImage(commandBuffer, bufferStaging, image, extent, arrayLayers, 0);

        if (mipmap) {
            CreateMipmaps(commandBuffer, image, extent, format, layout, mipLevels, 0, arrayLayers);
        } else {
            TransitionImageLayout(commandBuffer, image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout, aspect, mipLevels, 0, arrayLayers, 0);
        }
    }

    commandBuffer.submitIdle();

    updateDescriptor();
}
