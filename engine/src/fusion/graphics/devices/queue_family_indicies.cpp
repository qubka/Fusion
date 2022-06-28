#include "queue_family_indicies.hpp"

using namespace fe;

void QueueFamilyIndices::extract(const VkPhysicalDevice& physicalDevice) {
    uint32_t deviceQueueFamilyPropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &deviceQueueFamilyPropertyCount, nullptr);
    std::vector<VkQueueFamilyProperties> deviceQueueFamilyProperties(deviceQueueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &deviceQueueFamilyPropertyCount, deviceQueueFamilyProperties.data());

    for (uint32_t i = 0; i < deviceQueueFamilyPropertyCount; i++) {
        VkQueueFlags flags = deviceQueueFamilyProperties[i].queueFlags;
        // Check for graphics support.
        if (flags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamily = i;
            supportedQueues |= VK_QUEUE_GRAPHICS_BIT;
        }

        // Check for presentation support.
        //VkBool32 presentSupport;
        //vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, i, *surface, &presentSupport);

        if (deviceQueueFamilyProperties[i].queueCount > 0 /*&& presentSupport*/) {
            presentFamily = i;
        }

        // Check for compute support.
        if (flags & VK_QUEUE_COMPUTE_BIT) {
            computeFamily = i;
            supportedQueues |= VK_QUEUE_COMPUTE_BIT;
        }

        // Check for transfer support.
        if (flags & VK_QUEUE_TRANSFER_BIT) {
            transferFamily = i;
            supportedQueues |= VK_QUEUE_TRANSFER_BIT;
        }

        // Stop if found all required indices
        if (isComplete()) {
            break;
        }
    }

    if (graphicsFamily == VK_QUEUE_FAMILY_IGNORED)
        throw std::runtime_error("Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT");
}
