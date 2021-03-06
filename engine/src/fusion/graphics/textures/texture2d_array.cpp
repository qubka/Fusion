#include "texture2d_array.hpp"

#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/graphics/vku.hpp"
#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/filesystem/file_format.hpp"
#include "fusion/filesystem/file_system.hpp"

#include <gli/gli.hpp>

using namespace fe;

Texture2dArray::Texture2dArray(fs::path filepath, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, bool mipmap, bool load)
        : Texture{std::move(filepath),
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
                  0,
                  anisotropic,
                  mipmap} {
    if (!FileFormat::IsTextureStorageFile(path))
        throw std::runtime_error("Unsupported format for fast and single loading");

    if (load) {
        Texture2dArray::load();
    }
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
                  vku::getBlockParams(format).bytes,
                  anisotropic,
                  mipmap} {
}

Texture2dArray::Texture2dArray(const std::unique_ptr<Bitmap>& bitmap, uint32_t arrayLayers, VkFormat format,
                               VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter,
                               VkSamplerAddressMode addressMode, bool anisotropic, bool mipmap)
        : Texture{bitmap,
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
                  bitmap->getComponents(),
                  anisotropic,
                  mipmap} {
}

void Texture2dArray::setPixels(const float* pixels, uint32_t arrayLayer) {
    CommandBuffer commandBuffer{true};
	Buffer bufferStaging{extent.width * extent.height * components * arrayLayers, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pixels};
	CopyBufferToImage(commandBuffer, bufferStaging, image, extent, 1, arrayLayer);
    commandBuffer.submitIdle();
}

void Texture2dArray::load() {
#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif
    std::unique_ptr<gli::texture2d_array> texture;
    FileSystem::Read(path, [&texture](const uint8_t* data, size_t size) {
        texture = std::make_unique<gli::texture2d_array>(gli::load(reinterpret_cast<const char*>(data), size));
    });
#if FUSION_DEBUG
    LOG_DEBUG << "Texture2dArray: \"" << path << "\" loaded in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif

    const gli::texture2d_array& tex2DArray = *texture;
    if (tex2DArray.empty())
        throw std::runtime_error("Texture is empty");

    extent.width = static_cast<uint32_t>(tex2DArray.extent().x);
    extent.height = static_cast<uint32_t>(tex2DArray.extent().y);
    arrayLayers = static_cast<uint32_t>(tex2DArray.layers());
    mipLevels = static_cast<uint32_t>(tex2DArray.levels());
    components = static_cast<uint8_t>(component_count(tex2DArray.format()));

    if (extent.width == 0 || extent.height == 0)
        throw std::runtime_error("Width or height is empty");

    CommandBuffer commandBuffer{true};

    CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
    CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
    CreateImageView(image, view, viewType, format, aspect, mipLevels, 0, arrayLayers, 0);
    TransitionImageLayout(commandBuffer, image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspect, mipLevels, 0, arrayLayers, 0);

    Buffer bufferStaging{tex2DArray.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, tex2DArray.data()};

    // Setup buffer copy regions for each layer including all of it's miplevels
    std::vector<VkBufferImageCopy> bufferCopyRegions;
    bufferCopyRegions.reserve(arrayLayers * mipLevels);
    VkDeviceSize offset = 0;
    for (uint32_t layer = 0; layer < arrayLayers; layer++) {
        for (uint32_t level = 0; level < mipLevels; level++) {
            auto image = tex2DArray[layer][level];
            auto imageExtent = image.extent();

            VkBufferImageCopy region = {};
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
            bufferCopyRegions.push_back(region);

            // Increase offset into staging buffer for next level / face
            offset += image.size();
        }
    }

    vkCmdCopyBufferToImage(commandBuffer, bufferStaging, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());

    if (mipmap) {
        CreateMipmaps(commandBuffer, image, extent, format, layout, mipLevels, 0, arrayLayers);
    } else {
        TransitionImageLayout(commandBuffer, image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout, aspect, mipLevels, 0, arrayLayers, 0);
    }

    commandBuffer.submitIdle();

    updateDescriptor();
}
