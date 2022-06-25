#include "image2d_array.hpp"

#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"

using namespace fe;

Image2dArray::Image2dArray(const glm::uvec2& extent, uint32_t arrayLayers, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter,
                           VkSamplerAddressMode addressMode, bool anisotropic, bool mipmap)
    : Image{filter, addressMode, VK_SAMPLE_COUNT_1_BIT, layout, usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, format, 1, arrayLayers, {extent.x, extent.y, 1}}
    , anisotropic{anisotropic}
    , mipmap{mipmap}
{
	if (this->extent.width == 0 || this->extent.height == 0) {
		return;
	}

	//mipLevels = mipmap ? GetMipLevels(extent) : 1;

	CreateImage(image, memory, this->extent, format, samples, VK_IMAGE_TILING_OPTIMAL, this->usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
	CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
	CreateImageView(image, view, VK_IMAGE_VIEW_TYPE_2D_ARRAY, format, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
	TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, layout, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
}

Image2dArray::Image2dArray(std::unique_ptr<Bitmap>&& bitmap, uint32_t arrayLayers, VkFormat format, VkImageLayout layout, VkImageUsageFlags usage, VkFilter filter,
                           VkSamplerAddressMode addressMode, bool anisotropic, bool mipmap)
    : Image{filter, addressMode, VK_SAMPLE_COUNT_1_BIT, layout, usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, format, 1, arrayLayers, bitmap->getExtent()}
    , anisotropic{anisotropic}
    , mipmap{mipmap}
{
	if (extent.width == 0 || extent.height == 0) {
		return;
	}

	//mipLevels = mipmap ? GetMipLevels(extent) : 1;

	CreateImage(image, memory, extent, format, samples, VK_IMAGE_TILING_OPTIMAL, this->usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mipLevels, arrayLayers, VK_IMAGE_TYPE_2D);
	CreateImageSampler(sampler, filter, addressMode, anisotropic, mipLevels);
	CreateImageView(image, view, VK_IMAGE_VIEW_TYPE_2D_ARRAY, format, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
	TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);

	Buffer bufferStaging{ bitmap->getLength() * arrayLayers, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

	bufferStaging.map();
    bufferStaging.copy(bitmap->getData<void>());
	bufferStaging.unmap();

	std::vector<VkBufferImageCopy> bufferCopyRegions;
	bufferCopyRegions.reserve(arrayLayers);
	for (uint32_t layer = 0; layer < arrayLayers; layer++) {
		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = layer;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = extent;
		region.bufferOffset = 3 * extent.width * extent.height * layer;
		bufferCopyRegions.push_back(region);
	}
	CommandBuffer commandBuffer;
	vkCmdCopyBufferToImage(commandBuffer, bufferStaging, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());
	commandBuffer.submitIdle();
	
	TransitionImageLayout(image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, layout, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, 0, arrayLayers, 0);
}

void Image2dArray::setPixels(const float* pixels, uint32_t arrayLayer) {
	Buffer bufferStaging{extent.width * extent.height * 3, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

	bufferStaging.map();
    bufferStaging.copy(pixels);
	bufferStaging.unmap();

	CopyBufferToImage(bufferStaging, image, extent, 1, arrayLayer);
}
