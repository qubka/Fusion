#pragma once

#include "swapchain_support_details.hpp"

#include <volk.h>

namespace fe {
    class Instance;
    class LogicalDevice;
    class PhysicalDevice;
    class Window;

    class Surface {
        friend class Graphics;
    public:
        Surface(const Instance& instance, const PhysicalDevice& physicalDevice, const LogicalDevice& logicalDevice, const Window& window);
        ~Surface();
        NONCOPYABLE(Surface);

        operator bool() const { return surface != VK_NULL_HANDLE; }
        operator const VkSurfaceKHR&() const { return surface; }

        const SwapchainSupportDetails& getSwapchainSupportDetails();
        const VkSurfaceKHR& getSurface() const { return surface; }
        const Window& getWindow() const { return window; }

    private:
        const Instance& instance;
        const PhysicalDevice& physicalDevice;
        const Window& window;

        VkSurfaceKHR surface{ VK_NULL_HANDLE };
        SwapchainSupportDetails supportDetails;
    };
}
