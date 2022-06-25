#include "image2d.hpp"
#include "image.hpp"

#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/bitmaps/bitmap.hpp"

using namespace fe;

Image2d::Image2d(std::filesystem::path filename, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, bool mipmap, bool load)
    : Image{filter, addressMode, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_FORMAT_R8G8B8A8_UNORM, 1, 1, {0, 0, 1}}
    , filename{std::move(filename)}
    , anisotropic{anisotropic}
    , mipmap{mipmap}
{
	if (load) {
		Image2d::load();
	}
}

Image2d::Image2d(const glm::uvec2& extent, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter, VkSamplerAddressMode addressMode,
	VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
    : Image{filter, addressMode, samples, layout, usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, format, 1, 1, {extent.x, extent.y, 1}}
    , anisotropic{anisotropic}
    , mipmap{mipmap}
    , components{4}
{
    Image2d::load();
}

Image2d::Image2d(std::unique_ptr<Bitmap>&& bitmap, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter, VkSamplerAddressMode addressMode,
	VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
    : Image{filter, addressMode, samples, layout, usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, format, 1, 1, bitmap->getExtent()}
    , anisotropic{anisotropic}
    , mipmap{mipmap}
    , components{static_cast<uint32_t>(bitmap->getChannels())}
{
    Image2d::load(std::move(bitmap));
}

void Image2d::setPixels(const uint8_t* pixels, uint32_t layerCount, uint32_t baseArrayLayer) {
	Buffer bufferStaging{extent.width * extent.height * components * arrayLayers, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

	void* data;
	bufferStaging.mapMemory(&data);
	memcpy(data, pixels, bufferStaging.getSize());
	bufferStaging.unmapMemory();

	CopyBufferToImage(bufferStaging, image, extent, layerCount, baseArrayLayer);
}

void Image2d::load(std::unique_ptr<Bitmap> loadBitmap) {
	if (!filename.empty() && !loadBitmap) {
		loadBitmap = std::make_unique<Bitmap>(filename);
		extent = loadBitmap->getExtent();
		components = loadBitmap->getChannels();
	}
		
	if (extent.width == 0 || extent.height == 0)
		return;

	mipLevels = mipmap ? getMipLevels(extent) : 1;

	CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
	CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
	CreateImageView(image, view, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);

	if (loadBitmap || mipmap) {
		TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
	}

	if (loadBitmap) {
		Buffer bufferStaging{ loadBitmap->getLength(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

		uint8_t* data;
		bufferStaging.mapMemory(reinterpret_cast<void**>(&data));
		memcpy(data, loadBitmap->getData<void>(), bufferStaging.getSize());
		bufferStaging.unmapMemory();

		CopyBufferToImage(bufferStaging, image, extent, arrayLayers, 0);
	}

	if (mipmap) {
		CreateMipmaps(image, extent, format, layout, mipLevels, 0, arrayLayers);
	} else if (loadBitmap) {
		TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
	} else {
		TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, layout, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
	}
}
