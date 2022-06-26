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
    // Creates the surface.
    Graphics::CheckVk(window.createSurface(instance, nullptr, &surface));

    // Check for presentation support.
    VkBool32 presentSupport;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, logicalDevice.getPresentFamily(), surface, &presentSupport);

    if (!presentSupport)
        throw std::runtime_error("Present queue family does not have presentation support");
}

Surface::~Surface() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

Surface::SupportDetails Surface::getSupportDetails() const {
    SupportDetails details;

    Graphics::CheckVk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities));

    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
    details.formats.resize(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, details.formats.data());

    uint32_t physicalPresentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &physicalPresentModeCount, nullptr);
    details.presentModes.resize(physicalPresentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &physicalPresentModeCount, details.presentModes.data());

    return details;
}
