#pragma once

#include "Vulkan.hpp"

namespace Fusion {
    class FUSION_API SwapChain {
    public:
        SwapChain(Vulkan& vulkan, vk::Extent2D windowExtent);
        SwapChain(Vulkan& vulkan, vk::Extent2D windowExtent, std::shared_ptr<SwapChain> oldSwapChain);
        ~SwapChain();
        FE_NONCOPYABLE(SwapChain);

        const vk::Framebuffer& getFrameBuffer(size_t index) const { return swapChainFramebuffers[index]; }
        const vk::RenderPass& getRenderPass() const { return renderPass; }
        const vk::ImageView& getImageView(size_t index) const { return swapChainImageViews[index]; }
        const vk::Format& getSwapChainImageFormat() const { return swapChainImageFormat; }
        const vk::Extent2D& getSwapChainExtent() const { return swapChainExtent; }
        size_t imageCount() const { return swapChainImages.size(); }

        vk::Result acquireNextImage(uint32_t& imageIndex) const;
        vk::Result submitCommandBuffers(const vk::CommandBuffer& buffer, const uint32_t& imageIndex);

        bool compareSwapFormats(const SwapChain& other) const;
        void saveScreenshot(const std::string& path) const;

    private:
        void init();
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createDepthResources();
        void createFramebuffers();
        void createSyncObjects();

        Vulkan& vulkan;
        vk::Extent2D windowExtent;
        vk::Extent2D swapChainExtent;

        vk::SwapchainKHR swapChain;
        vk::RenderPass renderPass;

        std::vector<vk::Image> swapChainImages;
        vk::Format swapChainImageFormat;
        std::vector<vk::ImageView> swapChainImageViews;
        std::vector<vk::Framebuffer> swapChainFramebuffers;
        std::vector<vk::Semaphore> imageAvailableSemaphores;
        std::vector<vk::Semaphore> renderFinishedSemaphores;
        std::vector<vk::Fence> inFlightFences;
        std::vector<vk::Fence*> imagesInFlight;
        uint32_t currentFrame{0};

        vk::Image depthImage;
        vk::DeviceMemory depthImageMemory;
        vk::ImageView depthImageView;
        vk::Format swapChainDepthFormat;

        std::shared_ptr<SwapChain> oldSwapChain;

        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) const;
        vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) const;
        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) const;

    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    };
}
