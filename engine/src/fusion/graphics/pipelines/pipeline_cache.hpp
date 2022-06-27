#pragma once

#include <volk.h>

namespace fe {
    class LogicalDevice;
    class PipelineCache {
    public:
        PipelineCache(const LogicalDevice& logicalDevice);
        ~PipelineCache();

        operator bool() const { return pipelineCache != VK_NULL_HANDLE; }
        operator const VkPipelineCache&() const { return pipelineCache; }

        const VkPipelineCache& getPipelineCache() const { return pipelineCache; }

    private:
        const LogicalDevice& logicalDevice;

        VkPipelineCache pipelineCache{ VK_NULL_HANDLE };
    };
}
