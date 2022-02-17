#include "Pipeline.hpp"
#include "Vulkan.hpp"
#include "Mesh.hpp"

using namespace Fusion;

Pipeline::Pipeline(Vulkan& vulkan, const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo) : vulkan{vulkan} {
    createGraphicsPipeline(vertPath, fragPath, configInfo);
}

Pipeline::~Pipeline() {
    vulkan.getDevice().destroyShaderModule(vertShaderModule, nullptr);
    vulkan.getDevice().destroyShaderModule(fragShaderModule, nullptr);
    vulkan.getDevice().destroyPipeline(graphicsPipeline, nullptr);
}

void Pipeline::createGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo) {
    auto vertShaderCode = readFile(vertPath);
    auto fragShaderCode = readFile(fragPath);

    createShaderModule(vertShaderCode, vertShaderModule);
    createShaderModule(fragShaderCode, fragShaderModule);

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {{
        {
        {},
        vk::ShaderStageFlagBits::eVertex,
        vertShaderModule,
        "main"
        },
        {
        {},
        vk::ShaderStageFlagBits::eFragment,
        fragShaderModule,
        "main"
        }
    }};

    const auto& bindingDescriptions = configInfo.bindingDescriptions;
    const auto& attributeDescriptions = configInfo.attributeDescriptions;
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
    pipelineInfo.pViewportState = &configInfo.viewportInfo;
    pipelineInfo.pRasterizationState =&configInfo.rasterizationInfo;
    pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
    pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
    pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;
    pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
    pipelineInfo.layout = configInfo.pipelineLayout;
    pipelineInfo.renderPass = configInfo.renderPass;
    pipelineInfo.subpass = configInfo.subpass;
    pipelineInfo.basePipelineHandle = nullptr;

    auto result = vulkan.getDevice().createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &graphicsPipeline);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create pipeline layout!");
}

void Pipeline::createShaderModule(const std::vector<char>& code, vk::ShaderModule& shaderModule) {
    vk::ShaderModuleCreateInfo createInfo {
        {},
        code.size(),
        reinterpret_cast<const uint32_t*>(code.data())
    };
    auto result = vulkan.getDevice().createShaderModule(&createInfo, nullptr, &shaderModule);
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create shader module!");
}

std::vector<char> Pipeline::readFile(const std::string& path) {
    std::ifstream file {path, std::ios::ate | std::ios::binary};

    if (!file.is_open()) {
        FE_LOG_ERROR << "Failed to open file: " << path;
        return {};
    }

    size_t size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(size);

    file.seekg(0);
    file.read(buffer.data(), size);

    file.close();

    return buffer;
}

void Pipeline::bind(const vk::CommandBuffer& commandBuffer) const {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
}

void Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo& configInfo) {
    configInfo.inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    configInfo.viewportInfo.viewportCount = 1;
    configInfo.viewportInfo.pViewports = nullptr;
    configInfo.viewportInfo.scissorCount = 1;
    configInfo.viewportInfo.pScissors = nullptr;

    configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
    configInfo.rasterizationInfo.lineWidth = 1.0f;
    configInfo.rasterizationInfo.cullMode = vk::CullModeFlagBits::eFront;
    configInfo.rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
    configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;

    configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthCompareOp = vk::CompareOp::eLess;
    configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;

    configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    configInfo.multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

    configInfo.colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    configInfo.colorBlendAttachment.blendEnable = VK_FALSE;

    configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    configInfo.colorBlendInfo.logicOp = vk::LogicOp::eCopy;
    configInfo.colorBlendInfo.attachmentCount = 1;
    configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
    configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
    configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

    configInfo.dynamicStateEnables = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
    configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
    //configInfo.dynamicStateInfo.flags = 0;

    configInfo.bindingDescriptions = Mesh::Vertex::getBindingDescriptions();
    configInfo.attributeDescriptions = Mesh::Vertex::getAttributeDescriptions();
}
