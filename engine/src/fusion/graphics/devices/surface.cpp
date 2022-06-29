#include "surface.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/devices/instance.hpp"
#include "fusion/graphics/devices/logical_device.hpp"
#include "fusion/graphics/devices/physical_device.hpp"
#include "fusion/devices/devices.hpp"

using namespace fe;

Surface::Surface(const Instance& instance, const PhysicalDevice& physicalDevice, const LogicalDevice& logicalDevice, const Window& window)
    : instance{instance}
    , physicalDevice{physicalDevice}
    , window{window} {
    // Creates the surface
    VK_CHECK(window.createSurface(instance, nullptr, &surface));

    // Check for presentation support
    VkBool32 presentSupport;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, physicalDevice.getPresentFamily(), surface, &presentSupport);

    if (!presentSupport)
        throw std::runtime_error("Present queue family does not have presentation support");

    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities));

    format = getOptimalSurfaceFormat();
    presentMode = getOptimalPresentMode();
}

Surface::~Surface() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

const VkSurfaceCapabilitiesKHR& Surface::getCapabilities() {
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities));
    return capabilities;
}

VkExtent2D Surface::getExtent() const {
    if (capabilities.currentExtent.width == UINT32_MAX || capabilities.currentExtent.height == UINT32_MAX) {
        // Deadlock to wait until we get some valid data
        auto size = window.getSize();
        while (size.x == 0 || size.y == 0) {
            size = window.getSize();
            Devices::Get()->waitEvents();
        }

        return {
                std::max(capabilities.minImageExtent.width,
                         std::min(capabilities.maxImageExtent.width, size.x)),
                std::max(capabilities.minImageExtent.height,
                         std::min(capabilities.maxImageExtent.height, size.y))
        };
    } else {
        return capabilities.currentExtent;
    }
}

VkSurfaceFormatKHR Surface::getOptimalSurfaceFormat() const {
    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    if (surfaceFormatCount > 0)
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data());
    else
        throw std::runtime_error("Failed to find any surface format");

    // If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
    // there is no preferred format, so we assume VK_FORMAT_B8G8R8A8_UNORM
    if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
        return { VK_FORMAT_B8G8R8A8_UNORM, surfaceFormats[0].colorSpace };
    }

    // iterate over the list of available surface format and
    // check for the presence of VK_FORMAT_B8G8R8A8_UNORM
    for (const auto& availableFormat : surfaceFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM/* && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR*/) {
            return availableFormat;
        }
    }

    // in case VK_FORMAT_B8G8R8A8_UNORM is not available
    // select the first available color format
    return surfaceFormats[0];
}

VkPresentModeKHR Surface::getOptimalPresentMode() const {
    uint32_t physicalPresentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &physicalPresentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(physicalPresentModeCount);
    if (physicalPresentModeCount > 0)
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &physicalPresentModeCount, presentModes.data());
    else
        throw std::runtime_error("Failed to find any surface present mode");

    // Prefer mailbox mode if present, it's the lowest latency non-tearing present mode
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    if (!window.isVSync()) {
        for (const auto& mode : presentModes) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                bestMode = mode;
                break;
            }
            if (mode != VK_PRESENT_MODE_MAILBOX_KHR && mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                bestMode = mode;
            }
        }
    }

    return bestMode;
}
