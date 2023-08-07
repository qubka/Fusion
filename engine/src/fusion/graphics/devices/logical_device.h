#pragma once

namespace fe {
    class Instance;
    class PhysicalDevice;

    class FUSION_API LogicalDevice {
        friend class Graphics;
    public:
        LogicalDevice(const Instance& instance, const PhysicalDevice& physicalDevice, void* pNextChain = nullptr);
        ~LogicalDevice();
        NONCOPYABLE(LogicalDevice);

        operator bool() const { return logicalDevice != VK_NULL_HANDLE; }
        operator const VkDevice&() const { return logicalDevice; }

        const VkDevice& getLogicalDevice() const { return logicalDevice; }
        const VkPhysicalDeviceFeatures& getEnabledFeatures() const { return enabledFeatures; }
        const VkQueue& getGraphicsQueue() const { return graphicsQueue; }
        const VkQueue& getPresentQueue() const { return presentQueue; }
        const VkQueue& getComputeQueue() const { return computeQueue; }
        const VkQueue& getTransferQueue() const { return transferQueue; }

        static const std::vector<const char*> DeviceExtensions;

    private:
        const Instance& instance;
        const PhysicalDevice& physicalDevice;

        VkDevice logicalDevice{ VK_NULL_HANDLE };
        VkPhysicalDeviceFeatures enabledFeatures = {};

        VkPhysicalDeviceDescriptorIndexingFeaturesEXT physicalDeviceDescriptorIndexingFeatures = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT };

        VkQueue graphicsQueue{ VK_NULL_HANDLE };
        VkQueue presentQueue{ VK_NULL_HANDLE };
        VkQueue computeQueue{ VK_NULL_HANDLE };
        VkQueue transferQueue{ VK_NULL_HANDLE };
    };
}
