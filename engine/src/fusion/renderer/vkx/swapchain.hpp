/*
* Class wrapping access to the swap chain
*
* A swap chain is a collection of framebuffers used for rendering
* The swap chain images can then presented to the windowing system
*
* Copyright (C) 2022 by Nikita Ushakov
*/
#pragma once

#include "context.hpp"

namespace vkx {
struct SwapChainImage {
    vk::Image image;
    vk::ImageView view;
};

using FramebufferAttachment = Image;

class SwapChain {
    using Attachment = FramebufferAttachment;
    SwapChain(const vkx::Context& context)
        : context{context} {};
public:
    ~SwapChain() { destroy(); }
    SwapChain(const SwapChain& swapChain) = delete;

public:
    const vk::Framebuffer& getFrameBuffer(size_t index) const { return framebuffers[index]; };
    const vk::SwapchainKHR& getSwapChain() const { return swapChain; };
    const vk::ImageView& getImageView(size_t index) const { return images[index].view; };
    const vk::Format& getColorFormat() const { return colorFormat; };
    const vk::Format& getDepthFormat() const { return depthFormat; };
    const vk::Extent2D& getSurfaceExtent() const { return surfaceExtent; };
    //size_t imageCount() const { return swapChainImages.size(); };

    vk::Result acquireNextImage(uint32_t& imageIndex) const;
    vk::Result submitCommandBuffers(const vk::CommandBuffer& buffers, const uint32_t& imageIndex);

    /**
    * Create the swapchain and get its images with given width and height
    *
    * @param width Pointer to the width of the swapchain (may be adjusted to fit the requirements of the swapchain)
    * @param height Pointer to the height of the swapchain (may be adjusted to fit the requirements of the swapchain)
    * @param vsync (Optional) Can be used to force vsync-ed rendering (by using VK_PRESENT_MODE_FIFO_KHR as presentation mode)
    */
    static SwapChain* create(const vkx::Context& context, const vk::Extent2D& size, const vk::RenderPass& renderPass, bool vsync, const vk::SwapchainKHR& oldSwapChain = {nullptr});

private:
    const vkx::Context& context;
    const vk::Device& device{ context.device };
    const vk::Queue& queue{ context.queue };

    vk::Extent2D surfaceExtent;

    vk::SwapchainKHR swapChain;
    std::vector<SwapChainImage> images;
    vk::Format colorFormat { vk::Format::eB8G8R8A8Unorm };
    std::vector<vk::Framebuffer> framebuffers;
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;
    std::vector<vk::Fence*> imagesInFlight;
    uint32_t currentFrame{ 0 };

    Attachment depthStencil;
    vk::Format depthFormat { vk::Format::eUndefined };

    void destroy();

    void createSwapChain(const vk::Extent2D& size, const vk::SwapchainKHR& oldSwapChain, bool vsync);
    void createImages();
    void createDepthStencil();
    void createFramebuffers(const vk::RenderPass& renderPass);
    void createSyncObjects();

    static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    static vk::PresentModeKHR chooseSwapPresentMode(bool vsync, const std::vector<vk::PresentModeKHR>& availablePresentModes);
    static vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const vk::Extent2D& current);

public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
};
}  // namespace vkx