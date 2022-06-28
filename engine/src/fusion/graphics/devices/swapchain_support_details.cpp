#include "swapchain_support_details.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

SwapchainSupportDetails::SwapchainSupportDetails(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) {
    Graphics::CheckVk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities));

    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
    formats.resize(surfaceFormatCount);
    if (surfaceFormatCount > 0)
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, formats.data());

    uint32_t physicalPresentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &physicalPresentModeCount, nullptr);
    presentModes.resize(physicalPresentModeCount);
    if (physicalPresentModeCount > 0)
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &physicalPresentModeCount, presentModes.data());
}

VkSurfaceFormatKHR SwapchainSupportDetails::getOptimalSwapSurfaceFormat() const {
    // If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
    // there is no preferred format, so we assume VK_FORMAT_B8G8R8A8_UNORM
    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        return { VK_FORMAT_B8G8R8A8_UNORM, formats[0].colorSpace };
    }

    // iterate over the list of available surface format and
    // check for the presence of VK_FORMAT_B8G8R8A8_UNORM
    for (const auto& availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM/* && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR*/) {
            return availableFormat;
        }
    }

    // in case VK_FORMAT_B8G8R8A8_UNORM is not available
    // select the first available color format
    return formats[0];
}

VkPresentModeKHR SwapchainSupportDetails::getOptimalSwapPresentMode(bool isVSync) const {
    // Prefer mailbox mode if present, it's the lowest latency non-tearing present mode
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    if (!isVSync) {
        for (const auto& presentMode : presentModes) {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                bestMode = presentMode;
                break;
            }
            if (presentMode != VK_PRESENT_MODE_MAILBOX_KHR && presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                bestMode = presentMode;
            }
        }
    }

    return bestMode;
}

VkExtent2D SwapchainSupportDetails::getOptimalSwapChainExtent(const VkExtent2D& size) const {
    if (capabilities.currentExtent.width == UINT32_MAX || capabilities.currentExtent.height == UINT32_MAX) {
        return {
                std::max(capabilities.minImageExtent.width,
                         std::min(capabilities.maxImageExtent.width, size.width)),
                std::max(capabilities.minImageExtent.height,
                         std::min(capabilities.maxImageExtent.height, size.height))
        };
    } else {
        return capabilities.currentExtent;
    }
}

bool SwapchainSupportDetails::isSwapChainAdequate() const {
    return formats.empty() == false && presentModes.empty() == false;
}
