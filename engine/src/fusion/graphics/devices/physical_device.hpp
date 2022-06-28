#pragma once

#include "queue_family_indicies.hpp"

#include <volk.h>

namespace fe {
    class Instance;

    using DevicePickerFunction = std::function<VkPhysicalDevice(const std::vector<VkPhysicalDevice>&)>;

    class PhysicalDevice {
        friend class Graphics;
    public:
        explicit PhysicalDevice(const Instance& instance, const DevicePickerFunction& picker = [](const std::vector<VkPhysicalDevice>& devices) -> VkPhysicalDevice { return ChoosePhysicalDevice(devices); });
        ~PhysicalDevice() = default;
        NONCOPYABLE(PhysicalDevice);

        operator bool() const { return physicalDevice != VK_NULL_HANDLE; }
        operator const VkPhysicalDevice&() const { return physicalDevice; }

        const VkPhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
        const VkPhysicalDeviceProperties& getProperties() const { return properties; }
        const VkPhysicalDeviceFeatures& getFeatures() const { return features; }
        const VkPhysicalDeviceMemoryProperties& getMemoryProperties() const { return memoryProperties; }
        const VkSampleCountFlagBits& getMsaaSamples() const { return msaaSamples; }
        const QueueFamilyIndices& getQueueFamilyIndices() const { return queueFamilyIndices; };

    private:
        static VkPhysicalDevice ChoosePhysicalDevice(const std::vector<VkPhysicalDevice>& devices);
        static uint32_t ScorePhysicalDevice(const VkPhysicalDevice& device);
        VkSampleCountFlagBits getMaxUsableSampleCount() const;

        static void LogVulkanDevice(const VkPhysicalDeviceProperties& physicalDeviceProperties, const std::vector<VkExtensionProperties>& extensionProperties);

        const Instance& instance;

        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
        VkPhysicalDeviceProperties properties{};
        VkPhysicalDeviceFeatures features{};
        VkPhysicalDeviceMemoryProperties memoryProperties{};
        VkSampleCountFlagBits msaaSamples{ VK_SAMPLE_COUNT_1_BIT };

        QueueFamilyIndices queueFamilyIndices;
    };
}
