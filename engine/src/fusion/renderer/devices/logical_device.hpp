#pragma once

#include <volk.h>

namespace fe {
    class Instance;
    class PhysicalDevice;

    class LogicalDevice {
        friend class Graphics;
    public:
        LogicalDevice(const Instance& instance, const PhysicalDevice& physicalDevice);
        ~LogicalDevice();
        NONCOPYABLE(LogicalDevice);

        operator const VkDevice&() const { return logicalDevice; }

        const VkDevice& getLogicalDevice() const { return logicalDevice; }
        const VkPhysicalDeviceFeatures& getEnabledFeatures() const { return enabledFeatures; }
        const VkQueue& getGraphicsQueue() const { return graphicsQueue; }
        const VkQueue& getPresentQueue() const { return presentQueue; }
        const VkQueue& getComputeQueue() const { return computeQueue; }
        const VkQueue& getTransferQueue() const { return transferQueue; }
        uint32_t getGraphicsFamily() const { return graphicsFamily; }
        uint32_t getPresentFamily() const { return presentFamily; }
        uint32_t getComputeFamily() const { return computeFamily; }
        uint32_t getTransferFamily() const { return transferFamily; }

        static const std::vector<const char*> DeviceExtensions;

    private:
        void createQueueIndices();
        void createLogicalDevice();

        const Instance& instance;
        const PhysicalDevice& physicalDevice;

        VkDevice logicalDevice{ VK_NULL_HANDLE };
        VkPhysicalDeviceFeatures enabledFeatures{};

        VkQueueFlags supportedQueues{};
        uint32_t graphicsFamily{ 0 };
        uint32_t presentFamily{ 0 };
        uint32_t computeFamily{ 0 };
        uint32_t transferFamily{ 0 };

        VkQueue graphicsQueue{ VK_NULL_HANDLE };
        VkQueue presentQueue{ VK_NULL_HANDLE };
        VkQueue computeQueue{ VK_NULL_HANDLE };
        VkQueue transferQueue{ VK_NULL_HANDLE };
    };
}
