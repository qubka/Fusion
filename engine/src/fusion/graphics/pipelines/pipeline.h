#pragma once

#include "fusion/graphics/pipelines/shader.h"

namespace fe {
    class CommandBuffer;

    /**
     * @brief Class that is used to represent a pipeline.
     */
    class FUSION_API Pipeline {
    public:
        /**
         * @brief Represents position in the render structure, first value being the renderpass and second for subpass.
         */
        using Stage = std::pair<uint32_t, uint32_t>;

        Pipeline() = default;
        virtual ~Pipeline() = default;

        virtual void bindPipeline(const CommandBuffer& commandBuffer) const;

        virtual const Shader& getShader() const = 0;
        virtual const VkPipeline& getPipeline() const = 0;
        virtual const VkPipelineLayout& getPipelineLayout() const = 0;
        virtual const VkPipelineBindPoint& getPipelineBindPoint() const = 0;
        virtual const VkDescriptorSetLayout& getDescriptorSetLayout() const = 0;
        virtual bool isPushDescriptors() const = 0;
        virtual bool isIndexedDescriptors() const = 0;
    };
}
