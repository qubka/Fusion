#pragma once

namespace fe {
    class BindlessDescriptorSet {
        BindlessDescriptorSet(uint32_t binding);
        ~BindlessDescriptorSet();

        operator bool() const { return descriptorSet != VK_NULL_HANDLE; }
        operator const VkDescriptorSet&() const { return descriptorSet; }

        const VkDescriptorSetLayout& getDescriptorSetLayout() const { return descriptorSetLayout; }
        const VkDescriptorPool& getDescriptorPool() const { return descriptorPool; }
        const VkDescriptorSet& getDescriptorSet() const { return descriptorSet; }

    private:
        void createDescriptorLayout();
        void createDescriptorPool();
        void createDescriptors();

        VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
        VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };
        VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };
        uint32_t binding;
    };
}
