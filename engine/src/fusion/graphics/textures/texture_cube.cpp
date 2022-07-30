#include "texture_cube.hpp"

#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/filesystem/file_format.hpp"
#include "fusion/filesystem/file_system.hpp"

#include <gli/gli.hpp>

using namespace fe;

TextureCube::TextureCube(const fs::path& filepath, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, bool mipmap, bool load)
        : Texture{filepath,
                  filter,
                  addressMode,
                  VK_SAMPLE_COUNT_1_BIT,
                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_COLOR_BIT,
                  VK_IMAGE_VIEW_TYPE_CUBE,
                  VK_FORMAT_R8G8B8A8_UNORM,
                  1,
                  6,
                  { 0, 0, 1 },
                  anisotropic,
                  mipmap} {
    if (!FileFormat::IsTextureStorageFile(path))
        throw std::runtime_error("Unsupported format for fast and single loading");

    if (load) {
        TextureCube::load();
    }
}

TextureCube::TextureCube(const glm::uvec2& extent, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter,
                         VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
        : Texture{filter,
                  addressMode,
                  samples,
                  layout,
                  usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_COLOR_BIT,
                  VK_IMAGE_VIEW_TYPE_CUBE,
                  format,
                  1,
                  6,
                  { extent.x, extent.y, 1 },
                  anisotropic,
                  mipmap} {
}

TextureCube::TextureCube(const std::unique_ptr<Bitmap>& bitmap, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter,
                         VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
        : Texture{bitmap,
                  filter,
                  addressMode,
                  samples,
                  layout,
                  usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                  VK_IMAGE_ASPECT_COLOR_BIT,
                  VK_IMAGE_VIEW_TYPE_CUBE,
                  format,
                  1,
                  6,
                  vku::uvec3_cast(bitmap->getExtent()),
                  anisotropic,
                  mipmap} {
}

std::unique_ptr<Bitmap> TextureCube::getBitmap(uint32_t mipLevel) const {
	auto size = glm::uvec2{extent.width, extent.height} >> mipLevel;
    uint8_t components = vku::get_format_params(format).bytes;

	auto sizeSide = size.x * size.y * components;
	auto bitmap = std::make_unique<Bitmap>(glm::uvec2{size.x, size.y * arrayLayers}, format);
	auto offset = bitmap->getData<uint8_t>();

	for (uint32_t i = 0; i < 6; i++) {
		auto bitmapSide = Image::getBitmap(mipLevel, i);
		std::memcpy(offset, bitmapSide->getData<void>(), sizeSide);
		offset += sizeSide;
	}

	return bitmap;
}

void TextureCube::setPixels(const uint8_t* pixels, uint32_t layerCount, uint32_t baseArrayLayer) {
    uint8_t components = vku::get_format_params(format).bytes;
	Buffer bufferStaging{extent.width * extent.height * components * arrayLayers, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pixels};
	CopyBufferToImage(bufferStaging, image, extent, layerCount, baseArrayLayer);
}

void TextureCube::load() {
#if FUSION_DEBUG
    auto debugStart = DateTime::Now();
#endif
    std::unique_ptr<gli::texture_cube> texture;
    FileSystem::ReadBytes(path, [&texture](std::span<const uint8_t> buffer) {
        texture = std::make_unique<gli::texture_cube>(gli::load(reinterpret_cast<const char*>(buffer.data()), buffer.size()));
    });
#if FUSION_DEBUG
    LOG_DEBUG << "TextureCube: \"" << path << "\" loaded in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif

    const gli::texture_cube& texCube = *texture;
    if (texCube.empty())
        throw std::runtime_error("Texture is empty");

    extent.width = static_cast<uint32_t>(texCube.extent().x);
    extent.height = static_cast<uint32_t>(texCube.extent().y);
    // arrayLayers = 6
    mipLevels = static_cast<uint32_t>(texCube.levels());
    format = vku::convert_format(texCube.format());

    if (extent.width == 0 || extent.height == 0)
        throw std::runtime_error("Width or height is empty");

    if (arrayLayers != 6)
        throw std::runtime_error("Invalid amount of layers");

    CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, vku::convert_type(texCube.target()));
    CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
    CreateImageView(image, view, viewType, format, aspect, mipLevels, 0, arrayLayers, 0);

    TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aspect, mipLevels, 0, arrayLayers, 0);
    Buffer bufferStaging{texCube.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, texCube.data()};

    // Setup buffer copy regions for each layer including all of it's miplevels
    std::vector<VkBufferImageCopy> bufferCopyRegions;
    bufferCopyRegions.reserve(arrayLayers * mipLevels);
    VkDeviceSize offset = 0;
    for (uint32_t layer = 0; layer < arrayLayers; layer++) {
        for (uint32_t level = 0; level < mipLevels; level++) {
            auto image = texCube[layer][level];
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

    CommandBuffer commandBuffer{true};
    vkCmdCopyBufferToImage(commandBuffer, bufferStaging, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());
    commandBuffer.submitIdle();

    if (mipmap) {
        CreateMipmaps(image, extent, format, layout, mipLevels, 0, arrayLayers);
    } else {
        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout, aspect, mipLevels, 0, arrayLayers, 0);
    }

    updateDescriptor();
}