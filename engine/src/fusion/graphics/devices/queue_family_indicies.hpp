#pragma once

#include <volk.h>

namespace fe {
    class QueueFamilyIndices {
    public:
        void extract(const VkPhysicalDevice& device);

        uint32_t getGraphicsFamily() const { return graphicsFamily; }
        uint32_t getPresentFamily() const { return presentFamily; }
        uint32_t getComputeFamily() const { return computeFamily; }
        uint32_t getTransferFamily() const { return transferFamily; }

        VkQueueFlags getSupportedQueues() const { return supportedQueues; }

        bool isComplete() const {
            return graphicsFamily != VK_QUEUE_FAMILY_IGNORED && presentFamily != VK_QUEUE_FAMILY_IGNORED && computeFamily != VK_QUEUE_FAMILY_IGNORED && transferFamily != VK_QUEUE_FAMILY_IGNORED;
        }

    private:
        VkQueueFlags supportedQueues{};
        uint32_t graphicsFamily{ VK_QUEUE_FAMILY_IGNORED };
        uint32_t presentFamily{ VK_QUEUE_FAMILY_IGNORED };
        uint32_t computeFamily{ VK_QUEUE_FAMILY_IGNORED };
        uint32_t transferFamily{ VK_QUEUE_FAMILY_IGNORED };
    };
}
