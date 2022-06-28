#pragma once

#include <volk.h>

namespace fe {
    class PhysicalDevice;
    class Surface;
    class LogicalDevice;

    class Swapchain {
    public:
        Swapchain(const PhysicalDevice& physicalDevice, const LogicalDevice& logicalDevice, const Surface& surface, const Swapchain* oldSwapchain = nullptr);
        ~Swapchain();
        NONCOPYABLE(Swapchain);

        /**
         * Acquires the next image in the swapchain into the internal acquired image. The function will always waitEvents until the next image has been acquired by setting timeout to UINT64_MAX.
         * @param presentCompleteSemaphore A optional semaphore that is signaled when the image is ready for use.
         * @param fence A optional fence that is signaled once the previous command buffer has completed.
         * @return Result of the image acquisition.
         */
        VkResult acquireNextImage(const VkSemaphore& presentCompleteSemaphore = VK_NULL_HANDLE, VkFence fence = VK_NULL_HANDLE);

        /**
         * Queue an image for presentation using the internal acquired image for queue presentation.
         * @param presentQueue Presentation queue for presenting the image.
         * @param waitSemaphore A optional semaphore that is waited on before the image is presented.
         * @return Result of the queue presentation.
         */
        VkResult queuePresent(const VkQueue& presentQueue, const VkSemaphore& waitSemaphore = VK_NULL_HANDLE);

        bool isSameExtent(const VkExtent2D& extent2D) { return extent.width == extent2D.width && extent.height == extent2D.height; }

        operator bool() const { return swapchain != VK_NULL_HANDLE; }
        operator const VkSwapchainKHR&() const { return swapchain; }

        const VkExtent2D& getExtent() const { return extent; }
        uint32_t getImageCount() const { return imageCount; }
        const std::vector<VkImage>& getImages() const { return images; }
        const VkImage& getActiveImage() const { return images[activeImageIndex]; }
        const std::vector<VkImageView>& getImageViews() const { return imageViews; }
        const VkSwapchainKHR& getSwapchain() const { return swapchain; }
        uint32_t getActiveImageIndex() const { return activeImageIndex; }
        VkSurfaceFormatKHR getSurfaceFormat() const { return surfaceFormat; }
        VkPresentModeKHR getPresentMode() const { return presentMode; }
    private:
        const LogicalDevice& logicalDevice;

        VkExtent2D extent;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
        uint32_t imageCount{ 0 };
        VkPresentInfoKHR presentInfo = {};
        VkSwapchainKHR swapchain{ VK_NULL_HANDLE };
        uint32_t activeImageIndex{ 0 };
        VkSurfaceFormatKHR surfaceFormat = {};
        VkPresentModeKHR presentMode;
    };
}
