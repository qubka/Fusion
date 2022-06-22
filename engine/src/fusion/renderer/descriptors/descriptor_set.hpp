#pragma once

#include <volk.h>

namespace fe {
    class Pipeline;
    class CommandBuffer;
    class DescriptorSet {
    public:
        explicit DescriptorSet(const Pipeline& pipeline);
        ~DescriptorSet();

        static void update(const std::vector<VkWriteDescriptorSet>& descriptorWrites);

        void bindDescriptor(const CommandBuffer& commandBuffer) const;

        const VkDescriptorSet& getDescriptorSet() const { return descriptorSet; }

    private:
        VkPipelineLayout pipelineLayout;
        VkPipelineBindPoint pipelineBindPoint;
        VkDescriptorPool descriptorPool;
        VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };
    };
}
