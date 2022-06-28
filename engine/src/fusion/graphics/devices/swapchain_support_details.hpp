#pragma once

#include <volk.h>

namespace fe {
    class SwapchainSupportDetails {
    public:
        void extract(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);

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
