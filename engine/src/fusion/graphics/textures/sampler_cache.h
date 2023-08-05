#pragma once

namespace fe {
    class FUSION_API SamplerCache {
    public:
        explicit SamplerCache(VkDevice device);
        ~SamplerCache();
        NONCOPYABLE(SamplerCache);

        VkSampler createSampler(const VkSamplerCreateInfo& info) const;

    private:
        struct SamplerHash {
            size_t operator()(const VkSamplerCreateInfo& i) const;
        };

        mutable std::unordered_map<VkSamplerCreateInfo, VkSampler, SamplerHash> samplerCache;

        VkDevice device;
    };
}
