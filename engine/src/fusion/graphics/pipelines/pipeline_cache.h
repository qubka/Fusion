#pragma once

namespace fe {
    class FUSION_API PipelineCache {
    public:
        explicit PipelineCache(VkDevice device);
        ~PipelineCache();

        operator bool() const { return pipelineCache != VK_NULL_HANDLE; }
        operator const VkPipelineCache&() const { return pipelineCache; }

        const VkPipelineCache& getPipelineCache() const { return pipelineCache; }

    private:
        VkDevice device;

        VkPipelineCache pipelineCache{ VK_NULL_HANDLE };
    };
}
