#pragma once

#include <volk.h>

namespace fe {
    class SwapchainSupportDetails {
    public:
        SwapchainSupportDetails(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
        ~SwapchainSupportDetails() = default;

        bool isSwapChainAdequate() const;
        VkSurfaceFormatKHR getOptimalSwapSurfaceFormat() const;
        VkPresentModeKHR getOptimalSwapPresentMode(bool isVSync = false) const;
        VkExtent2D getOptimalSwapChainExtent(const VkExtent2D& size) const;
        const VkSurfaceCapabilitiesKHR& getCapabilities() const { return capabilities; }

    private:
        VkSurfaceCapabilitiesKHR capabilities = {};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
}
