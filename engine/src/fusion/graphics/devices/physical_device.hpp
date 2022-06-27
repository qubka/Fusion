#pragma once

#include <volk.h>

namespace fe {
    class Instance;

    using DevicePickerFunction = std::function<VkPhysicalDevice(const std::vector<VkPhysicalDevice>&)>;

    class PhysicalDevice {
        friend class Graphics;
    public:
        explicit PhysicalDevice(const Instance& instance, DevicePickerFunction picker = [](const std::vector<VkPhysicalDevice>& devices) -> VkPhysicalDevice { return ChoosePhysicalDevice(devices); });
        ~PhysicalDevice() = default;
        NONCOPYABLE(PhysicalDevice);

        operator const VkPhysicalDevice&() const { return physicalDevice; }

        const VkPhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
        const VkPhysicalDeviceProperties& getProperties() const { return properties; }
        const VkPhysicalDeviceFeatures& getFeatures() const { return features; }
        const VkPhysicalDeviceMemoryProperties& getMemoryProperties() const { return memoryProperties; }
        const VkSampleCountFlagBits& getMsaaSamples() const { return msaaSamples; }

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
    };
}
