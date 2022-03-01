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

struct SwapChain {
    using Attachment = FramebufferAttachment;

    const vkx::Context& context;
    const vk::Device& device{ context.device };
    const vk::Queue& queue{ context.queue };
    vk::Extent2D extent;
    vk::SwapchainKHR swapChain;
    std::vector<SwapChainImage> images;
    vk::Format colorFormat{ vk::Format::eB8G8R8A8Unorm };
    std::vector<vk::Framebuffer> framebuffers;
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;
    std::vector<vk::Fence*> imagesInFlight;
    uint32_t imageCount{ 0 };
    uint32_t currentFrame{ 0 };

    Attachment depthStencil;
    vk::Format depthFormat { vk::Format::eUndefined };

    SwapChain(const vkx::Context& context)
            : context{context} {};

    vk::Result acquireNextImage(uint32_t& imageIndex) const;
    vk::Result submitCommandBuffers(const std::vector<vk::CommandBuffer>& buffers, const uint32_t& imageIndex);

    // Creates an os specific surface
    // Tries to find a graphics and a present queue
    void create(const vk::Extent2D& size, const vk::RenderPass& renderPass, bool vsync) {
        createSwapChain(size, vsync);
        createImages();
        createDepthStencil();
        createFramebuffers(renderPass);
        createSyncObjects();
    }

    // Free all Vulkan resources used by the swap chain
    void destroy(const vk::SwapchainKHR& oldSwapChain = {nullptr});

private:
    void createSwapChain(const vk::Extent2D& size, bool vsync);
    void createImages();
    void createDepthStencil();
    void createFramebuffers(const vk::RenderPass& renderPass);
    void createSyncObjects();

    static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    static vk::PresentModeKHR chooseSwapPresentMode(bool vsync, const std::vector<vk::PresentModeKHR>& availablePresentModes);
    static vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, const vk::Extent2D& size);

public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
};
}  // namespace vkx