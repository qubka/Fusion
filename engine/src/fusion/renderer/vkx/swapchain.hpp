/*
* Class wrapping access to the swap chain
*
* A swap chain is a collection of framebuffers used for rendering
* The swap chain images can then presented to the windowing system
*
* Copyright (C) 2022 by Nikita Ushakov
*/
#pragma once

#include <vulkan/vulkan.hpp>

#include "context.hpp"

namespace vkx {
struct SwapChainImage {
    vk::Image image;
    vk::ImageView view;
};

using FramebufferAttachment = Image;

struct SwapChain {
    const vkx::Context& context;
    const vk::Device& device{ context.device };
    const vk::Queue& queue{ context.queue };

    vk::Extent2D windowExtent;
    vk::Extent2D surfaceExtent;

    vk::SwapchainKHR swapChain;
    vk::RenderPass renderPass;

    std::vector<SwapChainImage> images;
    vk::Format colorFormat { vk::Format::eB8G8R8A8Unorm };
    std::vector<vk::Framebuffer> framebuffers;
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;
    std::vector<vk::Fence*> imagesInFlight;
    uint32_t currentFrame{ 0 };

    Image depthStencil;
    vk::Format depthFormat { vk::Format::eUndefined };
    vk::SwapchainKHR oldSwapChain;

public:
    SwapChain(const vkx::Context& context, vk::Extent2D windowExtent, vk::SwapchainKHR oldSwapChain = vk::SwapchainKHR{nullptr})
            : context{context}
            , windowExtent{windowExtent}
            , oldSwapChain{oldSwapChain} {
        create();
    }
    void destroy();
    SwapChain(const SwapChain& swapChain) = delete;

    vk::Result acquireNextImage(uint32_t& imageIndex) const;
    vk::Result submitCommandBuffers(const vk::CommandBuffer& buffers, const uint32_t& imageIndex);

private:
    void create();
    void createSwapChain();
    void createImages();
    void createRenderPass();
    void createDepthStencil();
    void createFramebuffers();
    void createSyncObjects();

    vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
    vk::PresentModeKHR chooseSwapPresentMode(bool vsync, const std::vector<vk::PresentModeKHR>& availablePresentModes) const;
    vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;

public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
};
}  // namespace vkx