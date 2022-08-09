#include "pipeline_compute.hpp"
#include "shader_file.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/assets/asset_registry.hpp"

using namespace fe;

PipelineCompute::PipelineCompute(fs::path&& path, std::flat_map<std::string, Shader::SpecConstant>&& specConstants, std::vector<std::string>&& bindlessSets, bool pushDescriptors)
        : shader{}
        , path{std::move(path)}
        , specConstants{std::move(specConstants)}
        , bindlessSets{std::move(bindlessSets)}
        , pushDescriptors{pushDescriptors}
        , pipelineBindPoint{VK_PIPELINE_BIND_POINT_COMPUTE} {
#if FUSION_DEBUG
	auto debugStart = DateTime::Now();
#endif

	createShaderProgram();
	createDescriptorLayout();
	createPipelineLayout();
	createPipelineCompute();

#if FUSION_DEBUG
	LOG_DEBUG << "Pipeline Compute \"" << shader.getName() << "\" created in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}

PipelineCompute::~PipelineCompute() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	vkDestroyShaderModule(logicalDevice, shaderModule, nullptr);

	vkDestroyPipeline(logicalDevice, pipeline, nullptr);
}

void PipelineCompute::cmdRender(const CommandBuffer& commandBuffer, const glm::uvec2& extent) const {
	auto groupCountX = static_cast<uint32_t>(glm::ceil(static_cast<float>(extent.x) / static_cast<float>(*shader.getLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(glm::ceil(static_cast<float>(extent.y) / static_cast<float>(*shader.getLocalSizes()[1])));
	vkCmdDispatch(commandBuffer, groupCountX, groupCountY, 1);
}

void PipelineCompute::createShaderProgram() {
    auto shaderFile = AssetRegistry::Get()->get_or_emplace<ShaderFile>(path);

	auto shaderStage = shaderFile->getStage();
	shaderModule = shader.createShaderModule(shaderFile->getName(), shaderFile->getCode(), shaderStage);
    specialization = shader.createSpecialization(specConstants, shaderStage);

    shaderStageCreateInfo.stage = shaderStage;
	shaderStageCreateInfo.module = shaderModule;
	shaderStageCreateInfo.pName = "main";
    shaderStageCreateInfo.pSpecializationInfo = specialization ? &specialization->getSpecializationInfo() : nullptr;

    shader.createReflection();
}

void PipelineCompute::createDescriptorLayout() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	auto& descriptorSetLayouts = shader.getDescriptorSetLayouts();

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	descriptorSetLayoutCreateInfo.flags = pushDescriptors ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR : 0;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();
    descriptorSetLayout = Graphics::Get()->getDescriptorLayoutCache().createDescriptorLayout(descriptorSetLayoutCreateInfo);
}

void PipelineCompute::createPipelineLayout() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	auto pushConstantRanges = shader.getPushConstantRanges();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
    pipelineLayout = Graphics::Get()->getPipilineLayoutCache().createPipelineLayout(pipelineLayoutCreateInfo);
}

void PipelineCompute::createPipelineCompute() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
	auto pipelineCache = Graphics::Get()->getPipelineCache();

	VkComputePipelineCreateInfo pipelineCreateInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
	pipelineCreateInfo.stage = shaderStageCreateInfo;
	pipelineCreateInfo.layout = pipelineLayout;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;
	VK_CHECK(vkCreateComputePipelines(logicalDevice, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline));
}
