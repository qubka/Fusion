#include "pipeline_compute.hpp"

#include "fusion/renderer/graphics.hpp"
#include "fusion/utils/file.hpp"

using namespace fe;

PipelineCompute::PipelineCompute(std::filesystem::path shaderStage, std::vector<Shader::Define> defines, bool pushDescriptors)
    : shader{}
    , shaderStage{std::move(shaderStage)}
    , defines{std::move(defines)}
    , pushDescriptors{pushDescriptors}
    , pipelineBindPoint{VK_PIPELINE_BIND_POINT_COMPUTE}
{
	auto debugStart = Time::Now();

	createShaderProgram();
	createDescriptorLayout();
	createDescriptorPool();
	createPipelineLayout();
	createPipelineCompute();

	LOG_DEBUG << "Pipeline Compute " << this->shaderStage << " created in " << (Time::Now() - debugStart).asMilliseconds<float>() << "ms";
}

PipelineCompute::~PipelineCompute() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	vkDestroyShaderModule(logicalDevice, shaderModule, nullptr);

	vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
	vkDestroyPipeline(logicalDevice, pipeline, nullptr);
	vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
}

void PipelineCompute::cmdRender(const CommandBuffer& commandBuffer, const glm::uvec2& extent) const {
	auto groupCountX = static_cast<uint32_t>(std::ceil(static_cast<float>(extent.x) / static_cast<float>(*shader.getLocalSizes()[0])));
	auto groupCountY = static_cast<uint32_t>(std::ceil(static_cast<float>(extent.y) / static_cast<float>(*shader.getLocalSizes()[1])));
	vkCmdDispatch(commandBuffer, groupCountX, groupCountY, 1);
}

void PipelineCompute::createShaderProgram() {
	std::stringstream defineBlock;
	for (const auto& [defineName, defineValue] : defines)
		defineBlock << "#define " << defineName << " " << defineValue << '\n';

	auto fileLoaded = File::ReadTextFile(shaderStage);
	if (fileLoaded.empty())
		throw std::runtime_error("Could not create compute pipeline, missing shader stage");

	auto stageFlag = Shader::GetShaderStage(shaderStage);
	shaderModule = shader.createShaderModule(shaderStage, fileLoaded, defineBlock.str(), stageFlag);

	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfo.stage = stageFlag;
	shaderStageCreateInfo.module = shaderModule;
	shaderStageCreateInfo.pName = "main";

	shader.createReflection();
}

void PipelineCompute::createDescriptorLayout() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	auto descriptorSetLayouts = shader.getDescriptorSetLayouts();

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.flags = pushDescriptors ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR : 0;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();
	Graphics::CheckVk(vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));
}

void PipelineCompute::createDescriptorPool() {
	const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	auto descriptorPools = shader.getDescriptorPools();

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.maxSets = 8192; // 16384;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();
	Graphics::CheckVk(vkCreateDescriptorPool(logicalDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool));
}

void PipelineCompute::createPipelineLayout() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	auto pushConstantRanges = shader.getPushConstantRanges();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
	Graphics::CheckVk(vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));
}

void PipelineCompute::createPipelineCompute() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
	auto pipelineCache = Graphics::Get()->getPipelineCache();

	VkComputePipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stage = shaderStageCreateInfo;
	pipelineCreateInfo.layout = pipelineLayout;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;
	vkCreateComputePipelines(logicalDevice, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline);
}
