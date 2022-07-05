#pragma once

#include "pipeline.hpp"

namespace fe {
    /**
     * @brief Class that represents a compute compute pipeline.
     */
    class PipelineCompute : public Pipeline {
    public:
        /**
         * Creates a new compute pipeline.
         * @param shaderStage The shader file that will be loaded.
         * @param defines A list of defines added to the top of each shader.
         * @param pushDescriptors If no actual descriptor sets are allocated but instead pushed.
         */
        explicit PipelineCompute(std::filesystem::path shaderStage, std::vector<Shader::Define> defines = {}, bool pushDescriptors = false);
        ~PipelineCompute();

        void cmdRender(const CommandBuffer& commandBuffer, const glm::uvec2& extent) const;

        const std::filesystem::path& getShaderStage() const { return shaderStage; }
        const std::vector<Shader::Define>& getDefines() const { return defines; }
        bool isPushDescriptors() const override { return pushDescriptors; }
        const Shader& getShader() const override { return shader; }
        const VkDescriptorSetLayout& getDescriptorSetLayout() const override { return descriptorSetLayout; }
        const VkDescriptorPool& getDescriptorPool() const override { return descriptorPool; }
        const VkPipeline& getPipeline() const override { return pipeline; }
        const VkPipelineLayout& getPipelineLayout() const override { return pipelineLayout; }
        const VkPipelineBindPoint& getPipelineBindPoint() const override { return pipelineBindPoint; }

    private:
        void createShaderProgram();
        void createDescriptorLayout();
        void createDescriptorPool();
        void createPipelineLayout();
        void createPipelineCompute();

        std::filesystem::path shaderStage;
        std::vector<Shader::Define> defines;
        bool pushDescriptors;

        Shader shader;

        VkShaderModule shaderModule{ VK_NULL_HANDLE };
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};

        VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
        VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };

        VkPipeline pipeline{ VK_NULL_HANDLE };
        VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
        VkPipelineBindPoint pipelineBindPoint;
    };
}
