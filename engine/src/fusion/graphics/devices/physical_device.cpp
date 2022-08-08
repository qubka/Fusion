#include "physical_device.hpp"
#include "instance.hpp"

#include "fusion/graphics/graphics.hpp"

using namespace fe;

static const std::vector<VkSampleCountFlagBits> STAGE_FLAG_BITS = {
        VK_SAMPLE_COUNT_64_BIT,
        VK_SAMPLE_COUNT_32_BIT,
        VK_SAMPLE_COUNT_16_BIT,
        VK_SAMPLE_COUNT_8_BIT,
        VK_SAMPLE_COUNT_4_BIT,
        VK_SAMPLE_COUNT_2_BIT
};

PhysicalDevice::PhysicalDevice(const Instance& instance, uint32_t desiredDeviceIndex) : instance{instance} {
    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    if (physicalDeviceCount > 0)
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
    else
        throw std::runtime_error("Failed to find any physical GPU");

    physicalDevice = desiredDeviceIndex < physicalDeviceCount ? physicalDevices[desiredDeviceIndex] : ChoosePhysicalDevice(physicalDevices);

    if (!physicalDevice)
        throw std::runtime_error("Failed to find a suitable GPU");

    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    msaaSamples = getMaxUsableSampleCount();

    findQueueFamilyIndices();

    LOG_DEBUG << "Selected Physical Device: [" << properties.deviceID << "] \"" << properties.deviceName << "\"";
}

VkPhysicalDevice PhysicalDevice::ChoosePhysicalDevice(std::span<const VkPhysicalDevice> devices) {
    // Maps to hold devices and sort by rank.
    std::multimap<uint32_t, VkPhysicalDevice> rankedDevices;
    auto where = rankedDevices.end();

    // Iterates through all devices and rate their suitability.
    for (const auto& device : devices)
        where = rankedDevices.insert(where, { ScorePhysicalDevice(device), device });

    // Checks to make sure the best candidate scored higher than 0  rbegin points to last element of ranked devices(highest rated), first is its rating.
    if (rankedDevices.rbegin()->first > 0)
        return rankedDevices.rbegin()->second;

    return nullptr;
}

uint32_t PhysicalDevice::ScorePhysicalDevice(VkPhysicalDevice device) {
    uint32_t score = 0;

    // Checks if the requested extensions are supported
    uint32_t extensionPropertyCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionPropertyCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties(extensionPropertyCount);
    if (extensionPropertyCount > 0)
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionPropertyCount, extensionProperties.data());

    // Iterates through all extensions requested
    for (const char* currentExtension : LogicalDevice::DeviceExtensions) {
        bool extensionFound = false;

        // Checks if the extension is in the available extensions
        for (const auto& extension : extensionProperties) {
            if (strcmp(currentExtension, extension.extensionName) == 0) {
                extensionFound = true;
                break;
            }
        }

        // Returns a score of 0 if this device is missing a required extension
        if (!extensionFound)
            return 0;
    }

    // Obtain the device features and properties of the current device being rateds
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);

#if FUSION_DEBUG
    LogVulkanDevice(physicalDeviceProperties, extensionProperties);
#endif

    // Adds a large score boost for discrete GPUs (dedicated graphics cards)
    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;

    // Gives a higher score to devices with a higher maximum texture size
    score += physicalDeviceProperties.limits.maxImageDimension2D;
    return score;
}

VkSampleCountFlagBits PhysicalDevice::getMaxUsableSampleCount() const {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    auto counts = glm::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);

    for (const auto& sampleFlag : STAGE_FLAG_BITS) {
        if (counts & sampleFlag)
            return sampleFlag;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

void PhysicalDevice::findQueueFamilyIndices() {
    uint32_t queueFamilyPropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);
    std::vector<VkQueueFamilyProperties> deviceQueueFamilyProperties(queueFamilyPropertyCount);
    if (queueFamilyPropertyCount > 0)
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, deviceQueueFamilyProperties.data());

    for (const auto& [i, queueFamilyProperty] : enumerate(deviceQueueFamilyProperties)) {
        VkQueueFlags flags = queueFamilyProperty.queueFlags;

        // Check for graphics support
        if (flags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamily = i;
            supportedQueues |= VK_QUEUE_GRAPHICS_BIT;
        }

        // Check for presentation support
        //VkBool32 presentSupport;
        //vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, *surface, &presentSupport);

        if (queueFamilyProperty.queueCount > 0 /*&& presentSupport*/) {
            presentFamily = i;
        }

        // Check for compute support
        if (flags & VK_QUEUE_COMPUTE_BIT) {
            computeFamily = i;
            supportedQueues |= VK_QUEUE_COMPUTE_BIT;
        }

        // Check for transfer support
        if (flags & VK_QUEUE_TRANSFER_BIT) {
            transferFamily = i;
            supportedQueues |= VK_QUEUE_TRANSFER_BIT;
        }

        // Stop if found all required indices
        if (graphicsFamily != VK_QUEUE_FAMILY_IGNORED &&
            presentFamily != VK_QUEUE_FAMILY_IGNORED &&
            computeFamily != VK_QUEUE_FAMILY_IGNORED &&
            transferFamily != VK_QUEUE_FAMILY_IGNORED)
            break;
    }

    if (graphicsFamily == VK_QUEUE_FAMILY_IGNORED)
        throw std::runtime_error("Failed to find queue family supporting VK_QUEUE_GRAPHICS_BIT");

    uniqueFamilies.emplace(graphicsFamily);

    if (presentFamily != VK_QUEUE_FAMILY_IGNORED) {
        uniqueFamilies.emplace(presentFamily);
    } else {
        presentFamily = graphicsFamily;
    }

    if (computeFamily != VK_QUEUE_FAMILY_IGNORED) {
        uniqueFamilies.emplace(computeFamily);
    } else {
        computeFamily = graphicsFamily;
    }

    if (transferFamily != VK_QUEUE_FAMILY_IGNORED) {
        uniqueFamilies.emplace(transferFamily);
    } else {
        transferFamily = graphicsFamily;
    }
}

void PhysicalDevice::LogVulkanDevice(const VkPhysicalDeviceProperties& physicalDeviceProperties, std::span<const VkExtensionProperties> extensionProperties) {
    std::stringstream ss;
    switch (static_cast<int32_t>(physicalDeviceProperties.deviceType)) {
        case 1:
            ss << "Integrated";
            break;
        case 2:
            ss << "Discrete";
            break;
        case 3:
            ss << "Virtual";
            break;
        case 4:
            ss << "CPU";
            break;
        default:
            ss << "Other " << physicalDeviceProperties.deviceType;
    }

    ss << " Physical Device: " << physicalDeviceProperties.deviceID;
    switch (physicalDeviceProperties.vendorID) {
        case 0x8086:
            ss << " \"Intel\"";
            break;
        case 0x10DE:
            ss << " \"Nvidia\"";
            break;
        case 0x1002:
            ss << " \"AMD\"";
            break;
        default:
            ss << " \"" << physicalDeviceProperties.vendorID << '"';
    }

    ss << " \"" << physicalDeviceProperties.deviceName << "\"\n";

    uint32_t supportedVersion[3] = {
        VK_VERSION_MAJOR(physicalDeviceProperties.apiVersion),
        VK_VERSION_MINOR(physicalDeviceProperties.apiVersion),
        VK_VERSION_PATCH(physicalDeviceProperties.apiVersion)
    };
    ss << "API Version: " << supportedVersion[0] << '.' << supportedVersion[1] << '.' << supportedVersion[2] << '\n';

    ss << "Extensions: ";
    for (const auto& extension : extensionProperties)
        ss << extension.extensionName << ", ";

    ss << "\n\n";
    LOG_DEBUG << ss.str();
}
