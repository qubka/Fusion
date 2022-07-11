#include "image_cube.hpp"
#include "image.hpp"

#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/bitmaps/bitmap.hpp"

using namespace fe;

ImageCube::ImageCube(const glm::uvec2& extent, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter, VkSamplerAddressMode addressMode,
	VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
    : Image{filter, addressMode, samples, layout, usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, format, 1, 6, {extent.x, extent.y, 1}}
    , anisotropic(anisotropic)
    , mipmap{mipmap}
    , components{4} {
	ImageCube::load();
}

ImageCube::ImageCube(std::unique_ptr<Bitmap>&& bitmap, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter, VkSamplerAddressMode addressMode,
	VkSampleCountFlagBits samples, bool anisotropic, bool mipmap)
    : Image{filter, addressMode, samples, layout, usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, format, 1, 6, bitmap->getExtent()}
    , anisotropic{anisotropic}
    , mipmap{mipmap}
    , components{static_cast<uint32_t>(bitmap->getChannels())} {
	ImageCube::load(std::move(bitmap));
}

std::unique_ptr<Bitmap> ImageCube::getBitmap(uint32_t mipLevel) const {
	auto size = glm::uvec2{extent.width, extent.height} >> mipLevel;

	auto sizeSide = size.x * size.y * components;
	auto bitmap = std::make_unique<Bitmap>(glm::uvec2{size.x, size.y * arrayLayers}, static_cast<BitmapChannels>(components));
	auto offset = bitmap->getData<uint8_t>();

	for (uint32_t i = 0; i < 6; i++) {
		auto bitmapSide = Image::getBitmap(mipLevel, i);
		std::memcpy(offset, bitmapSide->getData<void>(), sizeSide);
		offset += sizeSide;
	}

	return bitmap;
}

void ImageCube::setPixels(const uint8_t* pixels, uint32_t layerCount, uint32_t baseArrayLayer) {
	Buffer bufferStaging{extent.width * extent.height * components * arrayLayers, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

	bufferStaging.map();
    bufferStaging.copy(pixels);
	bufferStaging.unmap();

	CopyBufferToImage(bufferStaging, image, extent, layerCount, baseArrayLayer);
}

void ImageCube::load(std::unique_ptr<Bitmap> loadBitmap) {
	if (!filename.empty() && !loadBitmap) {
		uint8_t* offset = nullptr;

		for (const auto& side : fileSides) {
			Bitmap bitmapSide{filename / (side + fileSuffix)};
			auto lengthSide = bitmapSide.getLength();

			if (!loadBitmap) {
				loadBitmap = std::make_unique<Bitmap>(std::make_unique<uint8_t[]>(lengthSide * arrayLayers), glm::uvec2{ bitmapSide.getWidth(), bitmapSide.getHeight() }, bitmapSide.getChannels(), bitmapSide.isHDR());
				offset = loadBitmap->getData<uint8_t>();
			}

			std::memcpy(offset, bitmapSide.getData<void>(), lengthSide);
			offset += lengthSide;
		}

		extent = loadBitmap->getExtent();
		components = static_cast<uint32_t>(loadBitmap->getChannels());
	}

	if (extent.width == 0 || extent.height == 0) {
		return;
	}

	mipLevels = mipmap ? getMipLevels(extent) : 1;

	CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
	CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
	CreateImageView(image, view, VK_IMAGE_VIEW_TYPE_CUBE, format, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);

	if (loadBitmap || mipmap) {
		TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
	}

	if (loadBitmap) {
		Buffer bufferStaging{ loadBitmap->getLength() * arrayLayers, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

		bufferStaging.map();
        bufferStaging.copy(loadBitmap->getData<void>());
		bufferStaging.unmap();

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