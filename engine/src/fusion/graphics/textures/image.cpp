#include "image.h"

#include "fusion/graphics/graphics.h"
#include "fusion/bitmaps/bitmap.h"
#include "fusion/graphics/buffers/buffer.h"
#include "fusion/graphics/commands/command_buffer.h"

using namespace fe;

static const float ANISOTROPY = 16.0f;

Image::Image(VkFilter filter, VkSamplerAddressMode addressMode, VkSampleCountFlagBits samples, VkImageLayout layout,
             VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImageViewType viewType, VkFormat format, const VkExtent3D& extent)
		: extent{extent}
		, samples{samples}
		, usage{usage}
		, format{format}
		, filter{filter}
        , addressMode{addressMode}
        , viewType{viewType}
        , aspect{aspect}
        , layout{layout} {
}

Image::~Image() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	vkDestroyImageView(logicalDevice, view, nullptr);
	vkFreeMemory(logicalDevice, memory, nullptr);
	vkDestroyImage(logicalDevice, image, nullptr);
}

std::unique_ptr<Bitmap> Image::getBitmap(uint32_t mipLevel, uint32_t arrayLayer) const {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

    auto size = glm::uvec2{extent.width, extent.height} >> mipLevel;
    VkImage dstImage;
    VkDeviceMemory dstImageMemory;

    CopyImage(image, dstImage, dstImageMemory, format, format, {size.x, size.y, 1}, layout, mipLevel, arrayLayer);

    VkImageSubresource dstImageSubresource = {};
    dstImageSubresource.aspectMask = aspect;
    dstImageSubresource.mipLevel = 0;
    dstImageSubresource.arrayLayer = 0;

    VkSubresourceLayout dstSubresourceLayout;
    vkGetImageSubresourceLayout(logicalDevice, dstImage, &dstImageSubresource, &dstSubresourceLayout);

    auto bitmap = std::make_unique<Bitmap>(std::make_unique<uint8_t[]>(dstSubresourceLayout.size), size, format);

    void* data;
    vkMapMemory(logicalDevice, dstImageMemory, dstSubresourceLayout.offset, dstSubresourceLayout.size, 0, &data);
    std::memcpy(bitmap->getData<void>(), data, dstSubresourceLayout.size);
    vkUnmapMemory(logicalDevice, dstImageMemory);

    vkFreeMemory(logicalDevice, dstImageMemory, nullptr);
    vkDestroyImage(logicalDevice, dstImage, nullptr);

	return bitmap;
}

VkDescriptorSetLayoutBinding Image::GetDescriptorSetLayout(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stage, uint32_t count) {
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding = binding;
    descriptorSetLayoutBinding.descriptorType = descriptorType;
    descriptorSetLayoutBinding.descriptorCount = count > 0 ? count : MAX_BINDLESS_RESOURCES;
    descriptorSetLayoutBinding.stageFlags = stage;
    descriptorSetLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;
    return descriptorSetLayoutBinding;
}

uint32_t Image::GetMipLevels(const VkExtent3D& extent) {
	return static_cast<uint32_t>(glm::floor(std::log2(glm::max(extent.width, glm::max(extent.height, extent.depth)))) + 1);
}

VkFormat Image::FindSupportedFormat(gsl::span<const VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();
	
	for (const auto& format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			return format;
		if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			return format;
	}

	return VK_FORMAT_UNDEFINED;
}

bool Image::HasDepth(VkFormat format) {
    switch (format) {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return true;
        default:
            return false;
    }
}

bool Image::HasStencil(VkFormat format) {
    switch (format) {
        case VK_FORMAT_S8_UINT:
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return true;
        default:
            return false;
    }
}

void Image::CreateImage(VkImage& image, VkDeviceMemory& memory, const VkExtent3D& extent, VkFormat format, VkSampleCountFlagBits samples, VkImageTiling tiling,
                        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t mipLevels, uint32_t arrayLayers, VkImageType imageType) {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	VkImageCreateInfo imageCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageCreateInfo.flags = arrayLayers == 6 ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	imageCreateInfo.imageType = imageType;
	imageCreateInfo.format = format;
	imageCreateInfo.extent = extent;
	imageCreateInfo.mipLevels = mipLevels;
	imageCreateInfo.arrayLayers = arrayLayers;
	imageCreateInfo.samples = samples;
	imageCreateInfo.tiling = tiling;
	imageCreateInfo.usage = usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VK_CHECK(vkCreateImage(logicalDevice, &imageCreateInfo, nullptr, &image));

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(logicalDevice, image, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = Buffer::FindMemoryType(memoryRequirements.memoryTypeBits, properties);
	VK_CHECK(vkAllocateMemory(logicalDevice, &memoryAllocateInfo, nullptr, &memory));

	VK_CHECK(vkBindImageMemory(logicalDevice, image, memory, 0));
}

void Image::CreateImageSampler(VkSampler& sampler, VkFilter filter, VkSamplerAddressMode addressMode, bool anisotropic, uint32_t mipLevels) {
	VkSamplerCreateInfo samplerCreateInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplerCreateInfo.magFilter = filter;
	samplerCreateInfo.minFilter = filter;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = addressMode;
	samplerCreateInfo.addressModeV = addressMode;
	samplerCreateInfo.addressModeW = addressMode;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = static_cast<VkBool32>(anisotropic);
	samplerCreateInfo.maxAnisotropy = (anisotropic && Graphics::Get()->getLogicalDevice().getEnabledFeatures().samplerAnisotropy) ? glm::min(ANISOTROPY, Graphics::Get()->getPhysicalDevice().getProperties().limits.maxSamplerAnisotropy) : 1.0f;
	//samplerCreateInfo.compareEnable = VK_FALSE;
	//samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = static_cast<float>(mipLevels);
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    sampler = Graphics::Get()->getSamplerCache().createSampler(samplerCreateInfo);
}

void Image::CreateImageView(VkImage image, VkImageView& imageView, VkImageViewType viewType, VkFormat format, VkImageAspectFlags imageAspect,
                            uint32_t mipLevels, uint32_t baseMipLevel, uint32_t layerCount, uint32_t baseArrayLayer) {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	VkImageViewCreateInfo imageViewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = viewType;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	imageViewCreateInfo.subresourceRange.aspectMask = imageAspect;
	imageViewCreateInfo.subresourceRange.baseMipLevel = baseMipLevel;
	imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
	imageViewCreateInfo.subresourceRange.layerCount = layerCount;
	VK_CHECK(vkCreateImageView(logicalDevice, &imageViewCreateInfo, nullptr, &imageView));
}

void Image::CreateMipmaps(VkImage image, const VkExtent3D& extent, VkFormat format, VkImageLayout dstImageLayout,
                          uint32_t mipLevels, uint32_t baseArrayLayer, uint32_t layerCount) {
    CommandBuffer commandBuffer{true};

    const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();

	// Get device properites for the requested Image format.
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);

	// Mip-chain generation requires support for blit source and destination
	FE_ASSERT(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT);
	FE_ASSERT(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);

	for (uint32_t i = 1; i < mipLevels; ++i) {
		VkImageMemoryBarrier barrier0 = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier0.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier0.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier0.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier0.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier0.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier0.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier0.image = image;
		barrier0.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier0.subresourceRange.baseMipLevel = i - 1;
		barrier0.subresourceRange.levelCount = 1;
		barrier0.subresourceRange.baseArrayLayer = baseArrayLayer;
		barrier0.subresourceRange.layerCount = layerCount;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier0);

		VkImageBlit imageBlit = {};
		imageBlit.srcOffsets[1] = {int32_t(extent.width >> (i - 1)), int32_t(extent.height >> (i - 1)), 1};
		imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.srcSubresource.mipLevel = i - 1;
		imageBlit.srcSubresource.baseArrayLayer = baseArrayLayer;
		imageBlit.srcSubresource.layerCount = layerCount;
		imageBlit.dstOffsets[1] = {int32_t(extent.width >> i), int32_t(extent.height >> i), 1};
		imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlit.dstSubresource.mipLevel = i;
		imageBlit.dstSubresource.baseArrayLayer = baseArrayLayer;
		imageBlit.dstSubresource.layerCount = layerCount;
		vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit, VK_FILTER_LINEAR);

		VkImageMemoryBarrier barrier1 = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
		barrier1.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier1.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier1.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier1.newLayout = dstImageLayout;
		barrier1.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier1.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier1.image = image;
		barrier1.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier1.subresourceRange.baseMipLevel = i - 1;
		barrier1.subresourceRange.levelCount = 1;
		barrier1.subresourceRange.baseArrayLayer = baseArrayLayer;
		barrier1.subresourceRange.layerCount = layerCount;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier1);
	}

	VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = dstImageLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = baseArrayLayer;
	barrier.subresourceRange.layerCount = layerCount;
	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    commandBuffer.submitIdle();
}

void Image::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout srcImageLayout, VkImageLayout dstImageLayout, VkImageAspectFlags imageAspect,
                                  uint32_t mipLevels, uint32_t baseMipLevel, uint32_t layerCount, uint32_t baseArrayLayer) {
    CommandBuffer commandBuffer{true};

    VkImageMemoryBarrier imageMemoryBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    imageMemoryBarrier.oldLayout = srcImageLayout;
    imageMemoryBarrier.newLayout = dstImageLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.srcAccessMask = LayoutToAccessMask(srcImageLayout, false);
    imageMemoryBarrier.dstAccessMask = LayoutToAccessMask(dstImageLayout, true);
    imageMemoryBarrier.subresourceRange.aspectMask = imageAspect;
    imageMemoryBarrier.subresourceRange.baseMipLevel = baseMipLevel;
    imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = baseArrayLayer;
    imageMemoryBarrier.subresourceRange.layerCount = layerCount;

    VkPipelineStageFlags sourceStage = 0;
    {
        if (imageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
            sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        } else if (imageMemoryBarrier.srcAccessMask != 0) {
            sourceStage = AccessFlagsToPipelineStage(imageMemoryBarrier.srcAccessMask,
                                                     VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                                                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                                                     VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        } else {
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }
    }

    VkPipelineStageFlags destinationStage = 0;
    {
        if (imageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
            destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        } else if (imageMemoryBarrier.dstAccessMask != 0) {
            destinationStage = AccessFlagsToPipelineStage(imageMemoryBarrier.dstAccessMask,
                                                          VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                                                          VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        } else {
            destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    commandBuffer.submitIdle();
}

void Image::InsertImageMemoryBarrier(VkCommandBuffer commandBuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout,
                                     VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageAspectFlags imageAspect,
                                     uint32_t mipLevels, uint32_t baseMipLevel, uint32_t layerCount, uint32_t baseArrayLayer) {
	VkImageMemoryBarrier imageMemoryBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	imageMemoryBarrier.srcAccessMask = srcAccessMask;
	imageMemoryBarrier.dstAccessMask = dstAccessMask;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = imageAspect;
	imageMemoryBarrier.subresourceRange.baseMipLevel = baseMipLevel;
	imageMemoryBarrier.subresourceRange.levelCount = mipLevels;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = baseArrayLayer;
	imageMemoryBarrier.subresourceRange.layerCount = layerCount;
	vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void Image::CopyBufferToImage(VkBuffer buffer, VkImage image, const VkExtent3D& extent, uint32_t layerCount, uint32_t baseArrayLayer) {
    CommandBuffer commandBuffer{true};

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = baseArrayLayer;
	region.imageSubresource.layerCount = layerCount;
	region.imageOffset = {0, 0, 0};
	region.imageExtent = extent;
	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    commandBuffer.submitIdle();
}

bool Image::CopyImage(VkImage srcImage, VkImage& dstImage, VkDeviceMemory& dstImageMemory, VkFormat srcFormat, VkFormat dstFormat,
                      const VkExtent3D& extent, VkImageLayout srcImageLayout, uint32_t mipLevel, uint32_t arrayLayer) {
    CommandBuffer commandBuffer{true};

    const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();
	auto swapchain = Graphics::Get()->getSwapchain(0);

	// Checks blit swapchain support.
	bool supportsBlit = true;
	VkFormatProperties formatProperties;

	// Check if the device supports blitting from optimal images (the swapchain images are in optimal format).
	vkGetPhysicalDeviceFormatProperties(physicalDevice, swapchain->getSurfaceFormat().format, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
		FE_LOG_WARNING("Device does not support blitting from optimal tiled images, using copy instead of blit!");
		supportsBlit = false;
	}

	// Check if the device supports blitting to linear images.
	vkGetPhysicalDeviceFormatProperties(physicalDevice, srcFormat, &formatProperties);

	if (!(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)) {
        FE_LOG_WARNING("Device does not support blitting to linear tiled images, using copy instead of blit!");
		supportsBlit = false;
	}

	CreateImage(dstImage, dstImageMemory, extent, dstFormat, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_LINEAR,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 1, 1, VK_IMAGE_TYPE_2D);

	// Transition destination image to transfer destination layout.
	InsertImageMemoryBarrier(commandBuffer, dstImage, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);

	// Transition image from previous usage to transfer source layout
	InsertImageMemoryBarrier(commandBuffer, srcImage, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT, srcImageLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevel, 1, arrayLayer);

	// If source and destination support blit we'll blit as this also does automatic format conversion (e.g. from BGR to RGB).
	if (supportsBlit) {
		// Define the region to blit (we will blit the whole swapchain image).
		VkOffset3D blitSize{ static_cast<int32_t>(extent.width), static_cast<int32_t>(extent.height), static_cast<int32_t>(extent.depth) };

		VkImageBlit imageBlitRegion = {};
		imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlitRegion.srcSubresource.mipLevel = mipLevel;
		imageBlitRegion.srcSubresource.baseArrayLayer = arrayLayer;
		imageBlitRegion.srcSubresource.layerCount = 1;
		imageBlitRegion.srcOffsets[1] = blitSize;
		imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBlitRegion.dstSubresource.mipLevel = 0;
		imageBlitRegion.dstSubresource.baseArrayLayer = 0;
		imageBlitRegion.dstSubresource.layerCount = 1;
		imageBlitRegion.dstOffsets[1] = blitSize;
		vkCmdBlitImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlitRegion, VK_FILTER_NEAREST);
	} else {
		// Otherwise use image copy (requires us to manually flip components).
		VkImageCopy imageCopyRegion = {};
		imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopyRegion.srcSubresource.mipLevel = mipLevel;
		imageCopyRegion.srcSubresource.baseArrayLayer = arrayLayer;
		imageCopyRegion.srcSubresource.layerCount = 1;
		imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopyRegion.dstSubresource.mipLevel = 0;
		imageCopyRegion.dstSubresource.baseArrayLayer = 0;
		imageCopyRegion.dstSubresource.layerCount = 1;
		imageCopyRegion.extent = extent;
		vkCmdCopyImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);
	}

	// Transition destination image to general layout, which is the required layout for mapping the image memory later on.
	InsertImageMemoryBarrier(commandBuffer, dstImage, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);

	// Transition back the image after the blit is done.
	InsertImageMemoryBarrier(commandBuffer, srcImage, VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcImageLayout,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_ASPECT_COLOR_BIT, 1, mipLevel, 1, arrayLayer);

    commandBuffer.submitIdle();

	return supportsBlit;
}

VkPipelineStageFlags Image::AccessFlagsToPipelineStage(VkAccessFlags accessFlags, VkPipelineStageFlags stageFlags) {
    VkPipelineStageFlags stages = 0;

    while (accessFlags != 0) {
        auto AccessFlag = static_cast<VkAccessFlagBits>(accessFlags & (~(accessFlags - 1)));
        FE_ASSERT(AccessFlag != 0 && (AccessFlag & (AccessFlag - 1)) == 0 && "Error");
        accessFlags &= ~AccessFlag;

        switch (AccessFlag) {
            case VK_ACCESS_INDIRECT_COMMAND_READ_BIT:
                stages |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
                break;

            case VK_ACCESS_INDEX_READ_BIT:
                stages |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
                break;

            case VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT:
                stages |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
                break;

            case VK_ACCESS_UNIFORM_READ_BIT:
                stages |= stageFlags | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                break;

            case VK_ACCESS_INPUT_ATTACHMENT_READ_BIT:
                stages |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;

            case VK_ACCESS_SHADER_READ_BIT:
                stages |= stageFlags | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                break;

            case VK_ACCESS_SHADER_WRITE_BIT:
                stages |= stageFlags | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                break;

            case VK_ACCESS_COLOR_ATTACHMENT_READ_BIT:
                stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                break;

            case VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT:
                stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                break;

            case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT:
                stages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                break;

            case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:
                stages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                break;

            case VK_ACCESS_TRANSFER_READ_BIT:
                stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;

            case VK_ACCESS_TRANSFER_WRITE_BIT:
                stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;

            case VK_ACCESS_HOST_READ_BIT:
                stages |= VK_PIPELINE_STAGE_HOST_BIT;
                break;

            case VK_ACCESS_HOST_WRITE_BIT:
                stages |= VK_PIPELINE_STAGE_HOST_BIT;
                break;

            case VK_ACCESS_MEMORY_READ_BIT:
                break;

            case VK_ACCESS_MEMORY_WRITE_BIT:
                break;

            default:
                FE_LOG_ERROR("Unknown access flag");
                break;
        }
    }
    return stages;
}

VkPipelineStageFlags Image::LayoutToAccessMask(VkImageLayout layout, bool isDestination) {
    VkPipelineStageFlags accessMask = 0;

    switch (layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            if (isDestination) {
                FE_LOG_ERROR("The new layout used in a transition must not be VK_IMAGE_LAYOUT_UNDEFINED.");
            }
            break;

        case VK_IMAGE_LAYOUT_GENERAL:
            accessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            accessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
            accessMask = VK_ACCESS_SHADER_READ_BIT; // VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            accessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            accessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            accessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            if (!isDestination) {
                accessMask = VK_ACCESS_HOST_WRITE_BIT;
            } else {
                FE_LOG_ERROR("The new layout used in a transition must not be VK_IMAGE_LAYOUT_PREINITIALIZED.");
            }
            break;

        case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
            accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
            accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
            accessMask = VK_ACCESS_MEMORY_READ_BIT;
            break;

        default:
            FE_LOG_ERROR("Unexpected image layout");
            break;
    }

    return accessMask;
}