#include "image2d.hpp"
#include "image.hpp"

#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/graphics/vku.hpp"
#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"

using namespace fe;

Image2d::Image2d(fs::path filepath, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, bool mipmap, bool load)
    : Image{filter, addressMode, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM, 1, 1, {0, 0, 1}}
    , filePath{std::move(filepath)}
    , anisotropic{anisotropic}
    , mipmap{mipmap} {
	if (load) {
		Image2d::load();
	}
}

Image2d::Image2d(const glm::uvec2& extent, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter, VkSamplerAddressMode addressMode,
	VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
    : Image{filter, addressMode, samples, layout, usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, format, 1, 1, {extent.x, extent.y, 1}}
    , components{vku::getBlockParams(format).bytes}
    , anisotropic{anisotropic}
    , mipmap{mipmap} {
    if (extent.x == 0 || extent.y == 0)
        throw std::runtime_error("Width or height is empty");

    mipLevels = mipmap ? getMipLevels(this->extent) : 1;

    CreateImage(image, memory, this->extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
    CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
    CreateImageView(image, view, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);

    if (mipmap) {
        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
        CreateMipmaps(image, this->extent, format, layout, mipLevels, 0, arrayLayers);
    } else {
        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, layout, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
    }
}

Image2d::Image2d(std::unique_ptr<Bitmap>&& bitmap, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter, VkSamplerAddressMode addressMode,
	VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
    : Image{filter, addressMode, samples, layout, usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, format, 1, 1, vku::uvec3_cast(bitmap->getExtent())}
    , components{bitmap->getComponents()}
    , anisotropic{anisotropic}
    , mipmap{mipmap} {
    if (extent.width == 0 || extent.height == 0)
        throw std::runtime_error("Width or height is empty");

    mipLevels = mipmap ? getMipLevels(extent) : 1;

    CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, this->usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
    CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
    CreateImageView(image, view, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
    TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);

    Buffer bufferStaging{bitmap->getLength() * arrayLayers, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bitmap->getData<void>()};
    CopyBufferToImage(bufferStaging, image, extent, arrayLayers, 0);

    if (mipmap) {
        CreateMipmaps(image, extent, format, layout, mipLevels, 0, arrayLayers);
    } else {
        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
    }
}

void Image2d::setPixels(const uint8_t* pixels, uint32_t layerCount, uint32_t baseArrayLayer) {
	Buffer bufferStaging{extent.width * extent.height * components * arrayLayers, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pixels};
	CopyBufferToImage(bufferStaging, image, extent, layerCount, baseArrayLayer);
}

void Image2d::load(std::unique_ptr<Bitmap> loadBitmap) {
    if (!filePath.empty() && !loadBitmap) {
        loadBitmap = std::make_unique<Bitmap>(filePath);
        extent =  vku::uvec3_cast(loadBitmap->getExtent());
        components = loadBitmap->getComponents();
    }

    if (extent.width == 0 || extent.height == 0)
        throw std::runtime_error("Width or height is empty");

    mipLevels = mipmap ? getMipLevels(extent) : 1;

    CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
    CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
    CreateImageView(image, view, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
    TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);

    Buffer bufferStaging{loadBitmap->getLength(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, loadBitmap->getData<void>()};
    CopyBufferToImage(bufferStaging, image, extent, arrayLayers, 0);

    if (mipmap) {
        CreateMipmaps(image, extent, format, layout, mipLevels, 0, arrayLayers);
    } else {
        TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
    }
}
