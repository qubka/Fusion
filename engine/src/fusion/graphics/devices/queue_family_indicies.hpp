#pragma once

#include <volk.h>

namespace fe {
    class QueueFamilyIndices {
    public:
        QueueFamilyIndices() = default;
        QueueFamilyIndices(const VkPhysicalDevice& device);
        ~QueueFamilyIndices() = default;

        uint32_t getGraphicsFamily() const { return graphicsFamily; }
        uint32_t getPresentFamily() const { return presentFamily; }
        uint32_t getComputeFamily() const { return computeFamily; }
        uint32_t getTransferFamily() const { return transferFamily; }

        VkQueueFlags getSupportedQueues() const { return supportedQueues; }
        const std::set<uint32_t>& getUniqueFamilies() const { return uniqueFamilies; };

    private:
        bool isComplete() const {
            return graphicsFamily != VK_QUEUE_FAMILY_IGNORED && presentFamily != VK_QUEUE_FAMILY_IGNORED && computeFamily != VK_QUEUE_FAMILY_IGNORED && transferFamily != VK_QUEUE_FAMILY_IGNORED;
        }

        VkQueueFlags supportedQueues{};
        uint32_t graphicsFamily{ VK_QUEUE_FAMILY_IGNORED };
        uint32_t presentFamily{ VK_QUEUE_FAMILY_IGNORED };
        uint32_t computeFamily{ VK_QUEUE_FAMILY_IGNORED };
        uint32_t transferFamily{ VK_QUEUE_FAMILY_IGNORED };
        std::set<uint32_t> uniqueFamilies;
    };
}