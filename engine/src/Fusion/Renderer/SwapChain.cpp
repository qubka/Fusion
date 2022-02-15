#include "SwapChain.hpp"

using namespace Fusion;

SwapChain::SwapChain(Vulkan& vulkan, vk::Extent2D windowExtent) : vulkan{vulkan}, windowExtent{windowExtent} {
    init();
}

SwapChain::SwapChain(Vulkan& vulkan, vk::Extent2D windowExtent, std::shared_ptr<SwapChain> previous)
        : vulkan{vulkan}, windowExtent{windowExtent}, oldSwapChain{std::move(previous)} {
    init();
    oldSwapChain = nullptr;
}

SwapChain::~SwapChain() {
    for (const auto& imageView : swapChainImageViews) {
        vulkan.getDevice().destroyImageView(imageView, nullptr);
    }

    vulkan.getDevice().destroyImageView(depthImageView, nullptr);
    vulkan.getDevice().destroyImage(depthImage, nullptr);
    vulkan.getDevice().freeMemory(depthImageMemory, nullptr);

    if (swapChain) {
        vulkan.getDevice().destroySwapchainKHR(swapChain, nullptr);
        swapChain = nullptr;
    }

    for (const auto& framebuffer : swapChainFramebuffers) {
        vulkan.getDevice().destroyFramebuffer(framebuffer, nullptr);
    }

    vulkan.getDevice().destroyRenderPass(renderPass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vulkan.getDevice().destroySemaphore(renderFinishedSemaphores[i], nullptr);
        vulkan.getDevice().destroySemaphore(imageAvailableSemaphores[i], nullptr);
        vulkan.getDevice().destroyFence(inFlightFences[i], nullptr);
    }
}

void SwapChain::init() {
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDepthResources();
    createFramebuffers();
    createSyncObjects();
}

void SwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = vulkan.getSwapChainSupport();

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo{
        vk::SwapchainCreateFlagsKHR(),
        vulkan.getSurface(),
        imageCount,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        extent,
        1, // imageArrayLayers
        vk::ImageUsageFlagBits::eColorAttachment
    };

    QueueFamilyIndices indices = vulkan.findPhysicalQueueFamilies();
    std::array<uint32_t, 2> queueFamilyIndices = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = static_cast<uint32_t >(queueFamilyIndices.size());
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = oldSwapChain == nullptr ? vk::SwapchainKHR(nullptr) : oldSwapChain->swapChain;

    auto result = vulkan.getDevice().createSwapchainKHR(&createInfo, nullptr, &swapChain);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create swap chain!");

    // we only specified a minimum number of images in the swap chain, so the implementation is
    // allowed to create a swap chain with more. That's why we'll first query the final number of
    // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
    // retrieve the handles.
    result = vulkan.getDevice().getSwapchainImagesKHR(swapChain, &imageCount, nullptr);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to get swap chain images count!");
    swapChainImages.resize(imageCount);
    result = vulkan.getDevice().getSwapchainImagesKHR(swapChain, &imageCount, swapChainImages.data());
    FE_ASSERT(result == vk::Result::eSuccess && "failed to get swap chain images!");

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
    swapChainDepthFormat = findDepthFormat();
}

vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const {
    if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
        return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
    }

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

vk::PresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) const {
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        } else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

vk::Extent2D SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const {
    if (capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max() ||
        capabilities.currentExtent.height == std::numeric_limits<std::uint32_t>::max()) {
        return {
                std::max(capabilities.minImageExtent.width,
                         std::min(capabilities.maxImageExtent.width, windowExtent.width)),
                std::max(capabilities.minImageExtent.height,
                         std::min(capabilities.maxImageExtent.height, windowExtent.height))
        };
    } else {
        return capabilities.currentExtent;
    }
}

void SwapChain::createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());

    for (int i = 0; i < swapChainImages.size(); i++) {
        vulkan.createImageView(swapChainImages[i], swapChainImageFormat, vk::ImageAspectFlagBits::eColor, swapChainImageViews[i]);
    }
}

void SwapChain::createRenderPass() {
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = vk::SampleCountFlagBits::e1;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentDescription depthAttachment{};
    depthAttachment.format = swapChainDepthFormat;
    depthAttachment.samples = vk::SampleCountFlagBits::e1;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

    std::array<vk::AttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    auto result = vulkan.getDevice().createRenderPass(&renderPassInfo, nullptr, &renderPass);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create render pass!");
}

void SwapChain::createDepthResources() {
    vulkan.createImage(
            swapChainExtent.width,
            swapChainExtent.height,
            swapChainDepthFormat,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            depthImage,
            depthImageMemory);

    vulkan.createImageView(depthImage, swapChainDepthFormat, vk::ImageAspectFlagBits::eDepth, depthImageView);
}

void SwapChain::createFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        std::array<vk::ImageView, 2> attachments = {
            swapChainImageViews[i],
            depthImageView
        };

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        auto result = vulkan.getDevice().createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers[i]);
        FE_ASSERT(result == vk::Result::eSuccess && "failed to create framebuffer!");
    }
}

void SwapChain::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapChainImages.size(), nullptr);

    vk::SemaphoreCreateInfo semaphoreInfo{};
    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    vk::Result result;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        result = vulkan.getDevice().createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
        FE_ASSERT(result == vk::Result::eSuccess && "failed to create semaphore objects for a frame!");
        result = vulkan.getDevice().createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
        FE_ASSERT(result == vk::Result::eSuccess && "failed to create semaphore objects for a frame!");
        result = vulkan.getDevice().createFence(&fenceInfo, nullptr, &inFlightFences[i]);
        FE_ASSERT(result == vk::Result::eSuccess && "failed to create fence objects for a frame!");
    }
}

vk::Format SwapChain::findDepthFormat() const {
    return vulkan.findSupportedFormat({
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint
    },
    vk::ImageTiling::eOptimal,
    vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

vk::Result SwapChain::acquireNextImage(uint32_t& imageIndex) const {
    auto result = vulkan.getDevice().waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
    FE_ASSERT(result == vk::Result::eSuccess && "failed to wait for fences");
    result = vulkan.getDevice().acquireNextImageKHR(swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], nullptr, &imageIndex);
    return result;
}

vk::Result SwapChain::submitCommandBuffers(const vk::CommandBuffer& buffers, const uint32_t& imageIndex) {
    vk::Fence& fence = inFlightFences[currentFrame];
    vk::Fence* image = imagesInFlight[imageIndex];
    if (image != nullptr) {
        auto result = vulkan.getDevice().waitForFences(1, image, VK_TRUE, std::numeric_limits<uint64_t>::max());
        FE_ASSERT(result == vk::Result::eSuccess && "failed to wait for fences");
    }
    imagesInFlight[imageIndex] = &fence;

    vk::SubmitInfo submitInfo{};

    vk::Semaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &buffers;

    vk::Semaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    auto result = vulkan.getDevice().resetFences(1, &fence);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to reset the fence");

    result = vulkan.getGraphicsQueue().submit(1, &submitInfo, fence);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to reset the fence");

    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vulkan.getPresentQueue().presentKHR(&presentInfo);
    //FE_ASSERT((result == vk::Result::eSuccess || result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) && "failed to present swap chain image!");

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

bool SwapChain::compareSwapFormats(const SwapChain& other) const {
    return other.swapChainDepthFormat == swapChainDepthFormat &&
           other.swapChainImageFormat == swapChainImageFormat;
}

void InsertImageMemoryBarrier(
        vk::CommandBuffer& commandBuffer,
        vk::Image& image,
        vk::AccessFlags srcAccessMask,
        vk::AccessFlags dstAccessMask,
        vk::ImageLayout oldImageLayout,
        vk::ImageLayout newImageLayout,
        vk::PipelineStageFlags srcStageMask,
        vk::PipelineStageFlags dstStageMask,
        vk::ImageSubresourceRange subresourceRange)
{
    vk::ImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    commandBuffer.pipelineBarrier(
            srcStageMask,
            dstStageMask,
            {},
            0,
            nullptr,
            0,
            nullptr,
            1,
            &imageMemoryBarrier);
}

void SwapChain::saveScreenshot(const std::string& path) const{
    //screenshotSaved = false;
    bool supportsBlit = true;
    
    uint32_t height = swapChainExtent.height;
    uint32_t width = swapChainExtent.width;

    // Check if the device supports blitting from optimal images (the swapchain images are in optimal format)
    // Check if the device supports blitting to linear images
    if (vulkan.findSupportedFormat({swapChainImageFormat}, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eBlitSrc) == vk::Format::eUndefined ||
        vulkan.findSupportedFormat({vk::Format::eR8G8B8A8Unorm}, vk::ImageTiling::eLinear, vk::FormatFeatureFlagBits::eBlitDst) == vk::Format::eUndefined) {
        FE_LOG_ERROR << "Device does not support blitting to optimal or linear tiled images, using copy instead of blit!";
        supportsBlit = false;
    }

    // Source for the copy is the last rendered swapchain image
    vk::Image srcImage = swapChainImages[currentFrame];

    // Create the image
    vk::Image dstImage;
    vk::DeviceMemory dstImageMemory;
    vulkan.createImage(width,
                       height,
                    vk::Format::eR8G8B8A8Unorm,
                    vk::ImageTiling::eLinear,
                    vk::ImageUsageFlagBits::eTransferDst,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                    dstImage,
                    dstImageMemory);

    // Do the actual blit from the swapchain image to our host visible destination image
    vk::CommandBuffer commandBuffer = vulkan.beginSingleTimeCommands();

    // Transition destination image to transfer destination layout
    InsertImageMemoryBarrier(
            commandBuffer,
            dstImage,
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eTransferWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal,
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

    // Transition swapchain image from present to transfer source layout
    InsertImageMemoryBarrier(
            commandBuffer,
            srcImage,
            vk::AccessFlagBits::eMemoryRead,
            vk::AccessFlagBits::eTransferRead,
            vk::ImageLayout::ePresentSrcKHR,
            vk::ImageLayout::eTransferSrcOptimal,
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

    // If source and destination support blit we'll blit as this also does automatic format conversion (e.g. from BGR to RGB)
    if (supportsBlit) {
        // Define the region to blit (we will blit the whole swapchain image)
        vk::Offset3D blitSize;
        blitSize.x = static_cast<int32_t>(width);
        blitSize.y = static_cast<int32_t>(height);
        blitSize.z = 1;
        vk::ImageBlit imageBlitRegion{};
        imageBlitRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        imageBlitRegion.srcSubresource.layerCount = 1;
        imageBlitRegion.srcOffsets[1] = blitSize;
        imageBlitRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        imageBlitRegion.dstSubresource.layerCount = 1;
        imageBlitRegion.dstOffsets[1] = blitSize;

        // Issue the blit command
        commandBuffer.blitImage(
                srcImage, 
                vk::ImageLayout::eTransferSrcOptimal,
                dstImage, 
                vk::ImageLayout::eTransferDstOptimal,
                1,
                &imageBlitRegion,
                vk::Filter::eNearest);
    } else {
        // Otherwise use image copy (requires us to manually flip components)
        vk::ImageCopy imageCopyRegion{};
        imageCopyRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        imageCopyRegion.srcSubresource.layerCount = 1;
        imageCopyRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        imageCopyRegion.dstSubresource.layerCount = 1;
        imageCopyRegion.extent.width = width;
        imageCopyRegion.extent.height = height;
        imageCopyRegion.extent.depth = 1;

        // Issue the copy command
        commandBuffer.copyImage(
                srcImage, 
                vk::ImageLayout::eTransferSrcOptimal,
                dstImage, 
                vk::ImageLayout::eTransferDstOptimal,
                1,
                &imageCopyRegion);
    }

    // Transition destination image to general layout, which is the required layout for mapping the image memory later on
    InsertImageMemoryBarrier(
            commandBuffer,
            dstImage,
            vk::AccessFlagBits::eTransferWrite,
            vk::AccessFlagBits::eMemoryRead,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eGeneral,
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

    // Transition back the swap chain image after the blit is done
    InsertImageMemoryBarrier(
            commandBuffer,
            srcImage,
            vk::AccessFlagBits::eTransferRead,
            vk::AccessFlagBits::eMemoryRead,
            vk::ImageLayout::eTransferSrcOptimal,
            vk::ImageLayout::ePresentSrcKHR,
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

    vulkan.endSingleTimeCommands(commandBuffer);

    // Get layout of the image (including row pitch)
    vk::ImageSubresource subResource { vk::ImageAspectFlagBits::eColor, 0, 0 };
    vk::SubresourceLayout subResourceLayout;
    vulkan.getDevice().getImageSubresourceLayout(dstImage, &subResource, &subResourceLayout);

    // Map image memory so we can start copying from it
    const char* data;
    vkMapMemory(vulkan.getDevice(), dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&data);
    data += subResourceLayout.offset;

    std::ofstream file(path, std::ios::out | std::ios::binary);

    // ppm header
    file << "P6\n" << width << "\n" << height << "\n" << 255 << "\n";

    // If source is BGR (destination is always RGB) and we can't use blit (which does automatic conversion), we'll have to manually swizzle color components
    bool colorSwizzle = false;
    // Check if source is BGR
    // Note: Not complete, only contains most common and basic BGR surface formats for demonstration purposes
    if (!supportsBlit) {
        std::vector<vk::Format> formatsBGR = { vk::Format::eB8G8R8A8Srgb, vk::Format::eB8G8R8A8Unorm, vk::Format::eB8G8R8A8Snorm };
        colorSwizzle = (std::find(formatsBGR.begin(), formatsBGR.end(), swapChainImageFormat) != formatsBGR.end());
    }

    // ppm binary pixel data
    for (uint32_t y = 0; y < height; y++) {
        unsigned int *row = (unsigned int*)data;
        for (uint32_t x = 0; x < width; x++) {
            if (colorSwizzle) {
                file.write((char*)row+2, 1);
                file.write((char*)row+1, 1);
                file.write((char*)row, 1);
            } else {
                file.write((char*)row, 3);
            }
            row++;
        }
        data += subResourceLayout.rowPitch;
    }
    file.close();

    std::cout << "Screenshot saved to disk" << std::endl;

    // Clean up resources
    vkUnmapMemory(vulkan.getDevice(), dstImageMemory);
    vkFreeMemory(vulkan.getDevice(), dstImageMemory, nullptr);
    vkDestroyImage(vulkan.getDevice(), dstImage, nullptr);

    //screenshotSaved = true;
}