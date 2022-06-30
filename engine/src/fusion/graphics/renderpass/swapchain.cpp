#include "swapchain.hpp"

#include "fusion/graphics/devices/physical_device.hpp"
#include "fusion/graphics/devices/logical_device.hpp"
#include "fusion/graphics/devices/surface.hpp"
#include "fusion/graphics/images/image.hpp"
#include "fusion/graphics/vku.hpp"

using namespace fe;

static const std::vector<VkCompositeAlphaFlagBitsKHR> COMPOSITE_ALPHA_FLAGS = {
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
};

Swapchain::Swapchain(const PhysicalDevice& physicalDevice, const LogicalDevice& logicalDevice, const Surface& surface, const Swapchain* oldSwapchain) : logicalDevice{logicalDevice} {
    const auto& capabilities = surface.getCapabilities();
    auto graphicsFamily = physicalDevice.getGraphicsFamily();
    auto presentFamily = physicalDevice.getPresentFamily();

    extent = surface.getExtent();
    surfaceFormat = surface.getFormat();
    presentMode = surface.getPresentMode();

    uint32_t desiredImageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 &&
        desiredImageCount > capabilities.maxImageCount) {
        desiredImageCount = capabilities.maxImageCount;
    }

    VkSurfaceTransformFlagsKHR preTransform;
	if (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		// We prefer a non-rotated transform.
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	} else {
		preTransform = capabilities.currentTransform;
	}

    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	for (const auto& compositeAlphaFlag : COMPOSITE_ALPHA_FLAGS) {
		if (capabilities.supportedCompositeAlpha & compositeAlphaFlag) {
			compositeAlpha = compositeAlphaFlag;
			break;
		}
	}

    VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = desiredImageCount;
	swapchainCreateInfo.imageFormat = surfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainCreateInfo.imageExtent = extent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = usage;
	if (graphicsFamily != presentFamily) {
		std::array<uint32_t, 2> queueFamily = { graphicsFamily, presentFamily };
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
		swapchainCreateInfo.pQueueFamilyIndices = queueFamily.data();
	} else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }
    swapchainCreateInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(preTransform);
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = compositeAlpha;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    if (oldSwapchain) {
        swapchainCreateInfo.oldSwapchain = oldSwapchain->swapchain;
        activeImageIndex = oldSwapchain->activeImageIndex;
    }
    VK_CHECK(vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapchain));

	VK_CHECK(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr));
    if (imageCount < 0)
        throw std::runtime_error("Failed to create swap chain images");

	images.resize(imageCount);
	imageViews.resize(imageCount);

	VK_CHECK(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, images.data()));

	for (uint32_t i = 0; i < imageCount; i++) {
		Image::CreateImageView(images[i], imageViews[i], VK_IMAGE_VIEW_TYPE_2D, surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, 1, 0);
	}
}

Swapchain::~Swapchain() {
	vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);

	for (const auto& imageView : imageViews) {
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
}

VkResult Swapchain::acquireNextImage(const VkSemaphore& presentCompleteSemaphore, VkFence fence) {
	if (fence != VK_NULL_HANDLE)
		VK_CHECK(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, UINT64_MAX));

	return vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX, presentCompleteSemaphore, VK_NULL_HANDLE, &activeImageIndex);
}

VkResult Swapchain::queuePresent(const VkQueue& presentQueue, const VkSemaphore& waitSemaphore) {
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &activeImageIndex;
	presentInfo.waitSemaphoreCount = waitSemaphore ? 1 : 0;
	presentInfo.pWaitSemaphores = &waitSemaphore;
	return vkQueuePresentKHR(presentQueue, &presentInfo);
}