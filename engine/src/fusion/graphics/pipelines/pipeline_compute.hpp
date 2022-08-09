#pragma once

#include "fusion/graphics/pipelines/pipeline.hpp"

namespace fe {
    /**
     * @brief Class that represents a compute compute pipeline.
     */
    class PipelineCompute : public Pipeline {
    public:
        /**
         * Creates a new compute pipeline.
         * @param path The shader file that will be loaded.
         * @param specConstants A map of specialization constants.
         * @param bindlessSets
         * @param pushDescriptors If no actual descriptor sets are allocated but instead pushed.
         */
        explicit PipelineCompute(fs::path&& path, std::flat_map<std::string, Shader::SpecConstant>&& specConstants = {}, std::vector<std::string>&& bindlessSets = {}, bool pushDescriptors = false);
        ~PipelineCompute() override;

        void cmdRender(const CommandBuffer& commandBuffer, const glm::uvec2& extent) const;

        const fs::path& getPath() const { return path; }
        const std::flat_map<std::string, Shader::SpecConstant>& getSpecConstants() const { return specConstants; }
        bool isPushDescriptors() const override { return pushDescriptors; }
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
        std::flat_map<std::string, Shader::SpecConstant> specConstants;
        std::vector<std::string> bindlessSets;
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
