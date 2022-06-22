#pragma once

#include <volk.h>

namespace fe {
    class Instance;

    class PhysicalDevice {
        friend class Graphics;
    public:
        explicit PhysicalDevice(const Instance& instance);
        ~PhysicalDevice() = default;
        NONCOPYABLE(PhysicalDevice);

        operator const VkPhysicalDevice&() const { return physicalDevice; }

        const VkPhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
        const VkPhysicalDeviceProperties& getProperties() const { return properties; }
        const VkPhysicalDeviceFeatures& getFeatures() const { return features; }
        const VkPhysicalDeviceMemoryProperties& getMemoryProperties() const { return memoryProperties; }
        const VkSampleCountFlagBits& getMsaaSamples() const { return msaaSamples; }

    private:
        VkPhysicalDevice choosePhysicalDevice(const std::vector<VkPhysicalDevice>& devices);
        static uint32_t scorePhysicalDevice(const VkPhysicalDevice& device);
        VkSampleCountFlagBits getMaxUsableSampleCount() const;

        static void logVulkanDevice(const VkPhysicalDeviceProperties& physicalDeviceProperties, const std::vector<VkExtensionProperties>& extensionProperties);

        const Instance& instance;

        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
        VkPhysicalDeviceProperties properties{};
        VkPhysicalDeviceFeatures features{};
        VkPhysicalDeviceMemoryProperties memoryProperties{};
        VkSampleCountFlagBits msaaSamples{ VK_SAMPLE_COUNT_1_BIT };
    };
}
