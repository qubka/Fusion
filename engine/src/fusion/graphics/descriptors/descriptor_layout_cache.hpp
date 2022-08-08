#pragma once

namespace fe {
    class DescriptorLayoutCache {
    public:
        explicit DescriptorLayoutCache(VkDevice device);
        ~DescriptorLayoutCache();
        NONCOPYABLE(DescriptorLayoutCache);

        VkDescriptorSetLayout createDescriptorLayout(const VkDescriptorSetLayoutCreateInfo& info) const;

    private:
        struct DescriptorLayoutInfo {
            VkDescriptorSetLayoutCreateFlags flags;
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            bool operator==(const DescriptorLayoutInfo& other) const;
        };

        struct DescriptorLayoutHash {
            size_t operator()(const DescriptorLayoutInfo& i) const;
        };

        mutable std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> layoutCache;

        VkDevice device;
    };
}