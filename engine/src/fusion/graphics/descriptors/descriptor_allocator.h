#pragma once

namespace fe {
    class FUSION_API DescriptorAllocator {
    public:
        explicit DescriptorAllocator(VkDevice device, uint32_t count, VkDescriptorPoolCreateFlags flags = 0);
        ~DescriptorAllocator();
        NONCOPYABLE(DescriptorAllocator);

        bool allocateDescriptor(VkDescriptorSetLayout layout, VkDescriptorSet& set, const void* next = nullptr) const;

        void resetPools() const;

    private:
        VkDescriptorPool grabPool() const;
        VkDescriptorPool createPool() const;

        mutable VkDescriptorPool currentPool{ VK_NULL_HANDLE };
        mutable std::vector<VkDescriptorPool> usedPools;
        mutable std::vector<VkDescriptorPool> freePools;

        VkDevice device;
        uint32_t count;
        VkDescriptorPoolCreateFlags flags;
    };
}
