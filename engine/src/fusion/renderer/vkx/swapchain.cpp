#include "swapchain.hpp"

using namespace vkx;

void SwapChain::destroy(const vk::SwapchainKHR& oldSwapChain) {
    for (const auto& image : images) {
        device.destroyImageView(image.view);
    }

    depthStencil.destroy();

    device.destroySwapchainKHR(oldSwapChain ? oldSwapChain : swapChain);

    for (const auto& framebuffer : framebuffers) {
        device.destroyFramebuffer(framebuffer);
    }
    for (const auto& semaphore : renderFinishedSemaphores) {
        device.destroySemaphore(semaphore);
    }
    for (const auto& semaphore : imageAvailableSemaphores) {
        device.destroySemaphore(semaphore);
    }
    for (const auto& fence : inFlightFences) {
        device.destroyFence(fence);
    }
    for (auto* image : imagesInFlight) {
        image = nullptr;
    }
}

void SwapChain::createSwapChain(const vk::Extent2D& size, bool vsync) {
    vk::SwapchainKHR oldSwapChain = swapChain;
    currentFrame = 0;

    vkx::Context::SwapChainSupportDetails swapChainSupport = context.getSwapChainSupport();

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(vsync, swapChainSupport.presentModes);
    extent = chooseSwapExtent(swapChainSupport.capabilities, size);

    // Determine the number of images
    uint32_t desiredNumberOfSwapchainImages =  swapChainSupport.capabilities.minImageCount + 1;
    if ((swapChainSupport.capabilities.maxImageCount > 0) && (desiredNumberOfSwapchainImages >  swapChainSupport.capabilities.maxImageCount)) {
        desiredNumberOfSwapchainImages =  swapChainSupport.capabilities.maxImageCount;
    }

    vk::SurfaceTransformFlagBitsKHR preTransform;
    if (swapChainSupport.capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
        preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    } else {
        preTransform = swapChainSupport.capabilities.currentTransform;
    }

    vk::SwapchainCreateInfoKHR swapchainCI;
    swapchainCI.surface = context.surface;
    swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
    swapchainCI.imageFormat = surfaceFormat.format;
    swapchainCI.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCI.imageExtent = extent;
    swapchainCI.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
    swapchainCI.preTransform = preTransform;
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageSharingMode = vk::SharingMode::eExclusive;
    swapchainCI.queueFamilyIndexCount = 0;
    swapchainCI.pQueueFamilyIndices = nullptr;
    swapchainCI.presentMode = presentMode;
    swapchainCI.oldSwapchain = oldSwapChain;
    swapchainCI.clipped = VK_TRUE;
    swapchainCI.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

    swapChain = device.createSwapchainKHR(swapchainCI);

    // If an existing sawp chain is re-created, destroy the old swap chain
    // This also cleans up all the presentable images
    if (oldSwapChain) {
        destroy(oldSwapChain);
    }

    // Find a suitable formats
    colorFormat = surfaceFormat.format;
    depthFormat = context.getSupportedDepthFormat();
}

void SwapChain::createImages() {
    vk::ImageViewCreateInfo colorAttachmentView;
    colorAttachmentView.format = colorFormat;
    colorAttachmentView.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    colorAttachmentView.subresourceRange.levelCount = 1;
    colorAttachmentView.subresourceRange.layerCount = 1;
    colorAttachmentView.viewType = vk::ImageViewType::e2D;

    // Get the swap chain images
    auto swapChainImages = device.getSwapchainImagesKHR(swapChain);
    imageCount = swapChainImages.size();

    // Get the swap chain buffers containing the image and imageview
    images.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; i++) {
        images[i].image = swapChainImages[i];
        colorAttachmentView.image = swapChainImages[i];
        images[i].view = device.createImageView(colorAttachmentView);
    }
}

void SwapChain::createDepthStencil() {
    vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

    vk::ImageCreateInfo depthStencilCI;
    depthStencilCI.imageType = vk::ImageType::e2D;
    depthStencilCI.extent = vk::Extent3D{ extent.width, extent.height, 1 };
    depthStencilCI.format = depthFormat;
    depthStencilCI.mipLevels = 1;
    depthStencilCI.arrayLayers = 1;
    depthStencilCI.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc;
    depthStencil = context.createImage(depthStencilCI);

    context.setImageLayout(depthStencil.image, aspect, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::ImageViewCreateInfo depthStencilView;
    depthStencilView.viewType = vk::ImageViewType::e2D;
    depthStencilView.format = depthFormat;
    depthStencilView.subresourceRange.aspectMask = aspect;
    depthStencilView.subresourceRange.levelCount = 1;
    depthStencilView.subresourceRange.layerCount = 1;
    depthStencilView.image = depthStencil.image;
    depthStencil.view = device.createImageView(depthStencilView);
}

void SwapChain::createFramebuffers(const vk::RenderPass& renderPass) {
    framebuffers.resize(imageCount);

    for (size_t i = 0; i < imageCount; i++) {
        std::array<vk::ImageView, 2> attachments = {
                images[i].view,
                depthStencil.view
        };

        vk::FramebufferCreateInfo framebufferCI;
        framebufferCI.renderPass = renderPass;
        framebufferCI.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferCI.pAttachments = attachments.data();
        framebufferCI.width = extent.width;
        framebufferCI.height = extent.height;
        framebufferCI.layers = 1;

        // Create frame buffers for every swap chain image
        framebuffers[i] = device.createFramebuffer(framebufferCI);
    }
}

void SwapChain::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(imageCount, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        imageAvailableSemaphores[i] = device.createSemaphore({});
        renderFinishedSemaphores[i] = device.createSemaphore({});
        inFlightFences[i] = device.createFence({vk::FenceCreateFlagBits::eSignaled});
    }
}

vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
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

vk::PresentModeKHR SwapChain::chooseSwapPresentMode(bool vsync, const std::vector<vk::PresentModeKHR>& availablePresentModes) {
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

vk::Extent2D SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const vk::Extent2D& size) {
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

vk::Result SwapChain::acquireNextImage(uint32_t& imageIndex) const {
    auto result = device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to wait for fences");
    }

    return device.acquireNextImageKHR(swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr, &imageIndex); /// use noexcept to handle OUT_OF_DATE
}

vk::Result SwapChain::submitCommandBuffers(const vk::CommandBuffer& buffers, const uint32_t& imageIndex) {
    vk::Fence& fence = inFlightFences[currentFrame];
    vk::Fence* image = imagesInFlight[imageIndex];
    if (image != nullptr) {
        auto result = device.waitForFences(1, image, VK_TRUE, UINT64_MAX);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to wait for fences");
        }
    }
    imagesInFlight[imageIndex] = &fence;

    vk::SubmitInfo submitInfo;

    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eBottomOfPipe };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffers;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];

    device.resetFences(fence);
    queue.submit(submitInfo, fence);

    vk::PresentInfoKHR presentInfo;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain;
    presentInfo.pImageIndices = &imageIndex;

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return queue.presentKHR(&presentInfo);
}