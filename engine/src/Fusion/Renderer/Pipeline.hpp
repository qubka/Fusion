#pragma once

#include "Vulkan.hpp"

namespace Fusion {
    struct FUSION_API PipelineConfigInfo {
        PipelineConfigInfo() = default;
        ~PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo(PipelineConfigInfo&&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(PipelineConfigInfo&&) = delete;

        std::vector<vk::VertexInputBindingDescription> bindingDescriptions{};
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
        vk::PipelineViewportStateCreateInfo viewportInfo{};
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo{};
        vk::PipelineMultisampleStateCreateInfo multisampleInfo{};
        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        vk::PipelineColorBlendStateCreateInfo colorBlendInfo{};
        vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{};
        std::vector<vk::DynamicState> dynamicStateEnables{};
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        vk::PipelineLayout pipelineLayout{nullptr};
        vk::RenderPass renderPass{nullptr};
        uint32_t subpass{0};
    };

    class FUSION_API Pipeline {
    public:
        Pipeline(Vulkan& vulkan,
                 const std::string& vertPath,
                 const std::string& fragPath,
                 const PipelineConfigInfo& configInfo);
        ~Pipeline();
        Pipeline(const Pipeline&) = delete;
        Pipeline(Pipeline&&) = delete;
        Pipeline& operator=(const Pipeline&) = delete;
        Pipeline& operator=(Pipeline&&) = delete;

        void bind(const vk::CommandBuffer& commandBuffer) const;

        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
    private:
        Vulkan& vulkan;
        vk::ShaderModule vertShaderModule;
        vk::ShaderModule fragShaderModule;
        vk::Pipeline graphicsPipeline;

        void createShaderModule(const std::vector<char>& code, vk::ShaderModule& shaderModule);
        void createGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo);

        static std::vector<char> readFile(const std::string& path);
    };
}
