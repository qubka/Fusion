#pragma once

namespace fe {
    class DescriptorAllocator {
    public:
        explicit DescriptorAllocator(VkDevice device);
        ~DescriptorAllocator();
        NONCOPYABLE(DescriptorAllocator);

        bool allocateDescriptor(VkDescriptorSetLayout layout, VkDescriptorSet& set) const;

        void resetPools() const;

    private:
        VkDescriptorPool grabPool() const;
        VkDescriptorPool createPool(uint32_t count, VkDescriptorPoolCreateFlags flags) const;

        mutable VkDescriptorPool currentPool{ VK_NULL_HANDLE };
        mutable std::vector<VkDescriptorPool> usedPools;
        mutable std::vector<VkDescriptorPool> freePools;

        VkDevice device;
    };
}
