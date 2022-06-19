#include "swapchain.hpp"

using namespace vkx;

vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    // If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
    // there is no preferred format, so we assume VK_FORMAT_B8G8R8A8_UNORM
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
        return { vk::Format::eB8G8R8A8Unorm, availableFormats[0].colorSpace };
    }

    // iterate over the list of available surface format and
    // check for the presence of VK_FORMAT_B8G8R8A8_UNORM
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm) {
            return availableFormat;
        }
    }

    // in case VK_FORMAT_B8G8R8A8_UNORM is not available
    // select the first available color format
    return availableFormats[0];
}

vk::PresentModeKHR chooseSwapPresentMode(bool vsync, const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    if (!vsync) {
        for (const auto& availablePresentMode: availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            } else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
                bestMode = availablePresentMode;
            }
        }
    }

    return bestMode;
}

vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const vk::Extent2D& size) {
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

void Swapchain::create(const vk::Extent2D& size, bool vsync) {
    vk::SwapchainKHR oldSwapchain = swapchain;
    currentImage = 0;

    vkx::Context::SwapChainSupportDetails swapChainSupport = context.getSwapChainSupport();

    surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    presentMode = chooseSwapPresentMode(vsync, swapChainSupport.presentModes);
    extent = chooseSwapExtent(swapChainSupport.capabilities, size);

    // Determine the number of images
    uint32_t desiredNumberOfSwapchainImages =  swapChainSupport.capabilities.minImageCount + 1;
    if ((swapChainSupport.capabilities.maxImageCount > 0) && (desiredNumberOfSwapchainImages >  swapChainSupport.capabilities.maxImageCount)) {
        desiredNumberOfSwapchainImages =  swapChainSupport.capabilities.maxImageCount;
    }

    if (swapChainSupport.capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
        preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    } else {
        preTransform = swapChainSupport.capabilities.currentTransform;
    }

    vk::SwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.surface = context.surface;
    swapchainCreateInfo.minImageCount = desiredNumberOfSwapchainImages;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = extent;
    swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
    swapchainCreateInfo.preTransform = preTransform;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.oldSwapchain = oldSwapchain;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.compositeAlpha = compositeAlpha;

    if (context.queueIndices.graphics != context.queueIndices.present) {
        std::array<uint32_t, 2> queueFamily = { context.queueIndices.graphics, context.queueIndices.present };
        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamily.size());
        swapchainCreateInfo.pQueueFamilyIndices = queueFamily.data();
    }

    swapchain = context.device.createSwapchainKHR(swapchainCreateInfo);

    // If an existing sawp chain is re-created, destroy the old swap chain
    // This also cleans up all the presentable images
    if (oldSwapchain) {
        destroy(oldSwapchain);
    }

    // Find a suitable formats
    colorFormat = surfaceFormat.format;
    depthFormat = context.getSupportedDepthFormat();

    // Create images
    vk::ImageViewCreateInfo colorAttachmentView;
    colorAttachmentView.format = colorFormat;
    colorAttachmentView.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    colorAttachmentView.subresourceRange.levelCount = 1;
    colorAttachmentView.subresourceRange.layerCount = 1;
    colorAttachmentView.viewType = vk::ImageViewType::e2D;

    // Get the swap chain images
    auto swapChainImages = context.device.getSwapchainImagesKHR(swapchain);
    imageCount = swapChainImages.size();

    // Get the swap chain buffers containing the image and imageview
    images.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; i++) {
        colorAttachmentView.image = swapChainImages[i];
        images[i].image = colorAttachmentView.image;
        images[i].view = context.device.createImageView(colorAttachmentView);
    }
}

void Swapchain::destroy(const vk::SwapchainKHR& oldSwapchain) {
    context.device.destroySwapchainKHR(oldSwapchain ? oldSwapchain : swapchain);

    for (const auto& image : images) {
        context.device.destroyImageView(image.view);
    }
    //images.clear();

    context.device.destroyFence(fenceImage);
}