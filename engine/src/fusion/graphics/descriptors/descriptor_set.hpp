#pragma once

namespace fe {
    class Pipeline;
    class CommandBuffer;
    class DescriptorSet {
    public:
        explicit DescriptorSet(const Pipeline& pipeline);
        ~DescriptorSet();

        static void updateDescriptor(std::span<const VkWriteDescriptorSet> descriptorWrites);
        void bindDescriptor(const CommandBuffer& commandBuffer) const;

        operator bool() const { return descriptorSet != VK_NULL_HANDLE; }
        operator const VkDescriptorSet&() const { return descriptorSet; }

        const VkDescriptorSet& getDescriptorSet() const { return descriptorSet; }

    private:
        VkPipelineLayout pipelineLayout;
        VkPipelineBindPoint pipelineBindPoint;

        VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };
    };
}
