#include "surface.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/devices/instance.hpp"
#include "fusion/graphics/devices/logical_device.hpp"
#include "fusion/graphics/devices/physical_device.hpp"

using namespace fe;

Surface::Surface(const Instance& instance, const PhysicalDevice& physicalDevice, const LogicalDevice& logicalDevice, const Window& window)
    : instance{instance}
    , physicalDevice{physicalDevice}
    , window{window}
{
    // Creates the surface
    Graphics::CheckVk(window.createSurface(instance, nullptr, &surface));

    // Check for presentation support
    VkBool32 presentSupport;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, logicalDevice.getPresentFamily(), surface, &presentSupport);

    if (!presentSupport)
        throw std::runtime_error("Present queue family does not have presentation support");

    // Update the swapchain support details for that surface
    supportDetails.extract(physicalDevice, surface);
}

Surface::~Surface() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

const SwapchainSupportDetails& Surface::getSwapchainSupportDetails() {
#if PLATFORM_LINUX
    // Doe to virtual surface consider re-extract information again
    supportDetails.extract(physicalDevice, surface);
#endif
    return supportDetails;
}
