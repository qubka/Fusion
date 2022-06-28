#pragma once

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
        uint32_t getGraphicsFamily() const { return graphicsFamily; }
        uint32_t getPresentFamily() const { return presentFamily; }
        uint32_t getComputeFamily() const { return computeFamily; }
        uint32_t getTransferFamily() const { return transferFamily; }
        VkQueueFlags getSupportedQueues() const { return supportedQueues; }
        const std::set<uint32_t>& getUniqueFamilies() const { return uniqueFamilies; };

    private:
        static VkPhysicalDevice ChoosePhysicalDevice(const std::vector<VkPhysicalDevice>& devices);
        static uint32_t ScorePhysicalDevice(const VkPhysicalDevice& device);
        VkSampleCountFlagBits getMaxUsableSampleCount() const;
        void findQueueFamilyIndices();

        static void LogVulkanDevice(const VkPhysicalDeviceProperties& physicalDeviceProperties, const std::vector<VkExtensionProperties>& extensionProperties);

        const Instance& instance;

        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
        VkPhysicalDeviceProperties properties{};
        VkPhysicalDeviceFeatures features{};
        VkPhysicalDeviceMemoryProperties memoryProperties{};
        VkSampleCountFlagBits msaaSamples{ VK_SAMPLE_COUNT_1_BIT };
        VkQueueFlags supportedQueues{};
        uint32_t graphicsFamily{ VK_QUEUE_FAMILY_IGNORED };
        uint32_t presentFamily{ VK_QUEUE_FAMILY_IGNORED };
        uint32_t computeFamily{ VK_QUEUE_FAMILY_IGNORED };
        uint32_t transferFamily{ VK_QUEUE_FAMILY_IGNORED };
        std::set<uint32_t> uniqueFamilies;
    };
}
