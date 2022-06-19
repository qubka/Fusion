#pragma once

#include "context.hpp"

namespace vkx {
    struct SwapchainImage {
        vk::Image image;
        vk::ImageView view;
    };

    struct Swapchain {
        const vkx::Context& context;

        vk::Extent2D extent;
        vk::PresentModeKHR presentMode;
        vk::PresentInfoKHR presentInfo;
        vk::SurfaceTransformFlagBitsKHR preTransform;
        std::vector<SwapchainImage> images;
        vk::SwapchainKHR swapchain;
        vk::SurfaceFormatKHR surfaceFormat;
        vk::Fence fenceImage;
        uint32_t imageCount{ 0 };
        uint32_t currentImage{ 0 };

        vk::Format colorFormat{ vk::Format::eB8G8R8A8Unorm };
        vk::Format depthFormat{ vk::Format::eUndefined };
        vk::CompositeAlphaFlagBitsKHR compositeAlpha{ vk::CompositeAlphaFlagBitsKHR::eOpaque };

        Swapchain(const vkx::Context& context) : context{context} {
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &swapchain;
            presentInfo.pImageIndices = &currentImage;
        }

        // Creates an os specific surface
        // Tries to find a graphics and a present queue
        void create(const vk::Extent2D& size, bool vsync);

        // Free all Vulkan resources used by the swap chain
        void destroy(const vk::SwapchainKHR& oldSwapchain = {});

        /**
         * Acquires the next image in the swapchain into the internal acquired image. The function will always wait until the next image has been acquired by setting timeout to UINT64_MAX.
         * @param presentCompleteSemaphore A optional semaphore that is signaled when the image is ready for use.
         * @param fence A optional fence that is signaled once the previous command buffer has completed.
         * @return Result of the image acquisition.
         */
        vk::Result acquireNextImage(const vk::Semaphore& presentCompleteSemaphore = {}, const vk::Fence& fence = {}) {
            if (fence) {
                auto result = context.device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
                if (result != vk::Result::eSuccess) {
                    throw std::runtime_error("failed to wait for fence: " + vk::to_string(result));
                }
            }

            return context.device.acquireNextImageKHR(swapchain, UINT64_MAX, presentCompleteSemaphore, nullptr, &currentImage);
        }

        /**
         * Queue an image for presentation using the internal acquired image for queue presentation.
         * @param waitSemaphore A optional semaphore that is waited on before the image is presented.
         * @return Result of the queue presentation.
         */
        vk::Result queuePresent(const vk::Semaphore& waitSemaphore = {}) {
            presentInfo.waitSemaphoreCount = waitSemaphore ? 1 : 0;
            presentInfo.pWaitSemaphores = &waitSemaphore;
            return context.presentQueue.presentKHR(&presentInfo);
        }
    };
}
