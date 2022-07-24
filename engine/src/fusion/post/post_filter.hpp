#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"

namespace fe {
    /**
     * @brief Represents a post effect pipeline.
     */
    class PostFilter : public Subrender {
    public:
        /**
         * Creates a new post filter.
         * @param pipelineStage The pipelines graphics stage.
         * @param shaderStages The pipelines shader stages.
         * @param defines A list of names that will be added as a define.
         */
        PostFilter(const Pipeline::Stage& pipelineStage, const std::vector<fs::path>& shaderStages, const std::vector<Shader::Define>& defines = {});
        virtual ~PostFilter() = default;

        void onUpdate() override {};

        const DescriptorsHandler& getDescriptorSet() const { return descriptorSet; }
        const PipelineGraphics& getPipeline() const { return pipeline; }

        const Descriptor* getAttachment(const std::string& descriptorName, const Descriptor* descriptor) const;
        const Descriptor* getAttachment(const std::string& descriptorName, const std::string& rendererAttachment) const;

        void setAttachment(const std::string& descriptorName, const Descriptor* descriptor);
        bool removeAttachment(const std::string& name);

    protected:
        /**
         * Used instead of {@link DescriptorsHandler#Push} in instances where a writeColor is the same as samplerColor in a shader.
         * By switching between what will be the input and output of each filter previous changes are available to the shader.
         * @param descriptorName1 The first descriptor in the shader.
         * @param descriptorName2 The second descriptor in the shader.
         * @param rendererAttachment1 The name of the renderers attachment that will be first option.
         * @param rendererAttachment2 The name of the renderers attachment that will be second option.
         */
        void pushConditional(const std::string& descriptorName1, const std::string& descriptorName2, const std::string& rendererAttachment1, const std::string& rendererAttachment2);

        inline static uint32_t GlobalSwitching = 0;

        DescriptorsHandler descriptorSet;
        PipelineGraphics pipeline;

        std::map<std::string, const Descriptor*> attachments;
    };
}
