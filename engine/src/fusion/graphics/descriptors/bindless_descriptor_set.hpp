#pragma once

namespace fe {
    class BindlessDescriptorSet {
    public:
        explicit BindlessDescriptorSet(uint32_t binding);
        ~BindlessDescriptorSet();

        operator bool() const { return descriptorSet != VK_NULL_HANDLE; }
        operator const VkDescriptorSet&() const { return descriptorSet; }

        const VkDescriptorSetLayout& getDescriptorSetLayout() const { return descriptorSetLayout; }
        const VkDescriptorSet& getDescriptorSet() const { return descriptorSet; }

    private:
        VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
        VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };
    };
}
