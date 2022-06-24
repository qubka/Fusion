#pragma once

#include "shader.hpp"

#include "fusion/graphics/commands/command_buffer.hpp"

namespace fe {
    /**
     * @brief Class that is used to represent a pipeline.
     */
    class Pipeline {
    public:
        /**
         * Represents position in the render structure, first value being the renderpass and second for subpass.
         */
        using Stage = std::pair<uint32_t, uint32_t>;

        Pipeline() = default;
        virtual ~Pipeline() = default;

        void bindPipeline(const CommandBuffer& commandBuffer) const {
            vkCmdBindPipeline(commandBuffer, getPipelineBindPoint(), getPipeline());
        }

        virtual const Shader& getShader() const = 0;
        virtual bool isPushDescriptors() const = 0;
        virtual const VkDescriptorSetLayout& getDescriptorSetLayout() const = 0;
        virtual const VkDescriptorPool& getDescriptorPool() const = 0;
        virtual const VkPipeline& getPipeline() const = 0;
        virtual const VkPipelineLayout& getPipelineLayout() const = 0;
        virtual const VkPipelineBindPoint& getPipelineBindPoint() const = 0;
    };
}
