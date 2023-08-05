#pragma once

namespace fe {
    class FUSION_API PipelineLayoutCache {
    public:
        explicit PipelineLayoutCache(VkDevice device);
        ~PipelineLayoutCache();
        NONCOPYABLE(PipelineLayoutCache);

        VkPipelineLayout createPipelineLayout(const VkPipelineLayoutCreateInfo& info) const;

    private:
        struct PipelineLayoutInfo {
            VkPipelineLayoutCreateFlags flags;
            std::vector<VkDescriptorSetLayout> setLayouts;
            std::vector<VkPushConstantRange> pushConstants;
            bool operator==(const PipelineLayoutInfo& other) const;
        };

        struct PipelineLayoutHash {
            size_t operator()(const PipelineLayoutInfo& i) const;
        };

        mutable std::unordered_map<PipelineLayoutInfo, VkPipelineLayout, PipelineLayoutHash> layoutCache;

        VkDevice device;
    };
}