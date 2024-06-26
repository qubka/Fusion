#pragma once

#include "fusion/graphics/pipelines/pipeline.h"

namespace fe {
    /**
     * @brief Class that represents a compute compute pipeline.
     */
    class FUSION_API PipelineCompute final : public Pipeline {
    public:
        /**
         * Creates a new compute pipeline.
         * @param path The shader file that will be loaded.
         * @param specConstants A map of specialization constants.
         * @param pushDescriptors If no actual descriptor sets are allocated but instead pushed.
         */
        explicit PipelineCompute(fs::path&& path, fst::unordered_flatmap<std::string, Shader::SpecConstant>&& specConstants = {}, bool pushDescriptors = false);
        ~PipelineCompute() override;

        void cmdRender(const CommandBuffer& commandBuffer, const glm::uvec2& extent) const;

        const fs::path& getPath() const { return path; }
        const fst::unordered_flatmap<std::string, Shader::SpecConstant>& getSpecConstants() const { return specConstants; }
        bool isPushDescriptors() const override { return pushDescriptors; }
        bool isIndexedDescriptors() const override { return false; }
        const Shader& getShader() const override { return shader; }
        const VkDescriptorSetLayout& getDescriptorSetLayout() const override { return descriptorSetLayout; }
        const VkPipeline& getPipeline() const override { return pipeline; }
        const VkPipelineLayout& getPipelineLayout() const override { return pipelineLayout; }
        const VkPipelineBindPoint& getPipelineBindPoint() const override { return pipelineBindPoint; }

    private:
        void createShaderProgram();
        void createDescriptorLayout();
        void createPipelineLayout();
        void createPipelineCompute();

        fs::path path;
        fst::unordered_flatmap<std::string, Shader::SpecConstant> specConstants;
        bool pushDescriptors;

        Shader shader;

        VkShaderModule shaderModule{ VK_NULL_HANDLE };
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };

        std::optional<Shader::Specialization> specialization;

        VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };

        VkPipeline pipeline{ VK_NULL_HANDLE };
        VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
        VkPipelineBindPoint pipelineBindPoint;
    };
}
