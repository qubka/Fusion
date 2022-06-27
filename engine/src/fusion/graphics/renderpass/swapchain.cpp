#include "swapchain.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

static const std::vector<VkCompositeAlphaFlagBitsKHR> COMPOSITE_ALPHA_FLAGS = {
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
    VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
};

Swapchain::Swapchain(const LogicalDevice& logicalDevice, const Surface& surface, const VkExtent2D& size, bool vsync, const Swapchain* oldSwapchain)
    : logicalDevice{logicalDevice}
{
	auto support = surface.getSupportDetails();
	auto graphicsFamily = logicalDevice.getGraphicsFamily();
	auto presentFamily = logicalDevice.getPresentFamily();

    extent = ChooseSwapExtent(support.capabilities, size);
    surfaceFormat = ChooseSwapSurfaceFormat(support.formats);
    presentMode = ChooseSwapPresentMode(vsync, support.presentModes);

    uint32_t desiredImageCount = support.capabilities.minImageCount + 1;
    if (support.capabilities.maxImageCount > 0 &&
        desiredImageCount > support.capabilities.maxImageCount) {
        desiredImageCount = support.capabilities.maxImageCount;
    }

    VkSurfaceTransformFlagsKHR preTransform;
	if (support.capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		// We prefer a non-rotated transform.
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	} else {
		preTransform = support.capabilities.currentTransform;
	}

    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	for (const auto& compositeAlphaFlag : COMPOSITE_ALPHA_FLAGS) {
		if (support.capabilities.supportedCompositeAlpha & compositeAlphaFlag) {
			compositeAlpha = compositeAlphaFlag;
			break;
		}
	}

    VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (support.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (support.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
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
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.preTransform = static_cast<VkSurfaceTransformFlagBitsKHR>(preTransform);
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.compositeAlpha = compositeAlpha;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = oldSwapchain ? oldSwapchain->swapchain : VK_NULL_HANDLE;

	if (graphicsFamily != presentFamily) {
		std::array<uint32_t, 2> queueFamily = { graphicsFamily, presentFamily };
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
		swapchainCreateInfo.pQueueFamilyIndices = queueFamily.data();
	}

	Graphics::CheckVk(vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapchain));

	Graphics::CheckVk(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, nullptr));
	images.resize(imageCount);
	imageViews.resize(imageCount);
	Graphics::CheckVk(vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, images.data()));

	for (uint32_t i = 0; i < imageCount; i++) {
		Image::CreateImageView(images[i], imageViews[i], VK_IMAGE_VIEW_TYPE_2D, surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT,
			1, 0, 1, 0);
	}

    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &activeImageIndex;
}

Swapchain::~Swapchain() {
	vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);

	for (const auto& imageView : imageViews) {
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
}

VkResult Swapchain::acquireNextImage(const VkSemaphore& presentCompleteSemaphore, VkFence fence) {
	if (fence != VK_NULL_HANDLE)
		Graphics::CheckVk(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, 1000000000));

	auto acquireResult = vkAcquireNextImageKHR(logicalDevice, swapchain, 1000000000, presentCompleteSemaphore, VK_NULL_HANDLE, &activeImageIndex);

	if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR && acquireResult != VK_ERROR_OUT_OF_DATE_KHR)
		throw std::runtime_error("Failed to acquire swapchain image");

	return acquireResult;
}

VkResult Swapchain::queuePresent(const VkQueue& presentQueue, const VkSemaphore& waitSemaphore) {
	presentInfo.waitSemaphoreCount = waitSemaphore ? 1 : 0;
	presentInfo.pWaitSemaphores = &waitSemaphore;
	return vkQueuePresentKHR(presentQueue, &presentInfo);
}

VkSurfaceFormatKHR Swapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    // If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
    // there is no preferred format, so we assume VK_FORMAT_B8G8R8A8_UNORM
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
        return { VK_FORMAT_B8G8R8A8_UNORM, availableFormats[0].colorSpace };
    }

    // iterate over the list of available surface format and
    // check for the presence of VK_FORMAT_B8G8R8A8_UNORM
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
            return availableFormat;
        }
    }

    // in case VK_FORMAT_B8G8R8A8_UNORM is not available
    // select the first available color format
    return availableFormats[0];
}

VkPresentModeKHR Swapchain::ChooseSwapPresentMode(bool vsync, const std::vector<VkPresentModeKHR>& physicalPresentModes) {
    // Prefer mailbox mode if present, it's the lowest latency non-tearing present mode
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    if (!vsync) {
        for (const auto& presentMode : physicalPresentModes) {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                bestMode = presentMode;
                break;
            }
            if (presentMode != VK_PRESENT_MODE_MAILBOX_KHR && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                bestMode = presentMode;
            }
        }
    }

    return bestMode;
}

VkExtent2D Swapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& size) {
    if (capabilities.currentExtent.width == UINT32_MAX || capabilities.currentExtent.height == UINT32_MAX) {
        return {
                std::max(capabilities.minImageExtent.width,
                         std::min(capabilities.maxImageExtent.width, size.width)),
                std::max(capabilities.minImageExtent.height,
                         std::min(capabilities.maxImageExtent.height, size.height))
        };
    } else {
        return capabilities.currentExtent;
    }
}