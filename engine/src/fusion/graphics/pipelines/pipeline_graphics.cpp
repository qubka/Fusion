#include "pipeline_graphics.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/render_stage.hpp"
#include "fusion/utils/date_time.hpp"
#include "fusion/filesystem/file_system.hpp"

using namespace fe;

PipelineGraphics::PipelineGraphics(Stage stage, std::vector<std::filesystem::path> shaderStages, std::vector<Vertex::Input> vertexInputs, std::vector<Shader::Define> defines,
	Mode mode, Depth depth, VkPrimitiveTopology topology, VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, Blend blend, bool depthBiasEnabled,
    float depthBiasConstantFactor, float depthBiasSlopeFactor, float depthBiasClamp, float lineWidth, bool transparencyEnabled, bool pushDescriptors)
    : shader{}
    , stage{std::move(stage)}
    , shaderStages{std::move(shaderStages)}
    , vertexInputs{std::move(vertexInputs)}
    , defines{std::move(defines)}
    , mode{mode}
    , depth{depth}
    , topology{topology}
    , polygonMode{polygonMode}
    , cullMode{cullMode}
    , frontFace{frontFace}
    , blend{blend}
    , depthBiasEnabled{depthBiasEnabled}
    , depthBiasConstantFactor{depthBiasConstantFactor}
    , depthBiasSlopeFactor{depthBiasSlopeFactor}
    , depthBiasClamp{depthBiasClamp}
    , lineWidth{lineWidth}
    , transparencyEnabled{transparencyEnabled}
    , pushDescriptors{pushDescriptors}
    , pipelineBindPoint{VK_PIPELINE_BIND_POINT_GRAPHICS} {

#if FUSION_DEBUG
	auto debugStart = DateTime::Now();
#endif

	std::sort(this->vertexInputs.begin(), this->vertexInputs.end());

	createShaderProgram();
	createDescriptorLayout();
	createDescriptorPool();
	createPipelineLayout();
	createAttributes();

	switch (mode) {
	case Mode::Polygon:
		createPipelinePolygon();
		break;
	case Mode::MRT:
		createPipelineMrt();
		break;
	default:
		throw std::runtime_error("Unknown pipeline mode");
	}

#if FUSION_DEBUG
	LOG_DEBUG << "Pipeline Graphics: " << std::quoted(shader.getName()) << " loaded in " << (DateTime::Now() - debugStart).asMilliseconds<float>() << "ms";
#endif
}

PipelineGraphics::~PipelineGraphics() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	for (const auto& shaderModule : modules)
		vkDestroyShaderModule(logicalDevice, shaderModule, nullptr);

	vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
	vkDestroyPipeline(logicalDevice, pipeline, nullptr);
	vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout, nullptr);
}

const ImageDepth* PipelineGraphics::getDepthStencil(const std::optional<uint32_t>& stage) const {
	return Graphics::Get()->getRenderStage(stage ? *stage : this->stage.first)->getDepthStencil();
}

const Image2d* PipelineGraphics::getImage(size_t index, const std::optional<uint32_t>& stage) const {
	return Graphics::Get()->getRenderStage(stage ? *stage : this->stage.first)->getFramebuffers()->getAttachment(index);
}

const RenderArea& PipelineGraphics::getRenderArea(const std::optional<uint32_t>& stage) const {
	return Graphics::Get()->getRenderStage(stage ? *stage : this->stage.first)->getRenderArea();
}

void PipelineGraphics::createShaderProgram() {
	std::stringstream defineBlock;
	for (const auto& [defineName, defineValue] : defines)
		defineBlock << "#define " << defineName << " " << defineValue << '\n';

	for (const auto& shaderStage : shaderStages) {
		auto fileLoaded = FileSystem::ReadText(shaderStage);
		if (fileLoaded.empty())
			throw std::runtime_error("Could not create pipeline, missing shader stage");

		auto stageFlag = Shader::GetShaderStage(shaderStage);
		auto shaderModule = shader.createShaderModule(shaderStage, fileLoaded, defineBlock.str(), stageFlag);

		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {};
		pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo.stage = stageFlag;
		pipelineShaderStageCreateInfo.module = shaderModule;
		pipelineShaderStageCreateInfo.pName = "main";
		stages.push_back(pipelineShaderStageCreateInfo);
		modules.push_back(shaderModule);
	}

	shader.createReflection();
}

void PipelineGraphics::createDescriptorLayout() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	auto& descriptorSetLayouts = shader.getDescriptorSetLayouts();

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.flags = pushDescriptors ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR : 0;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();
	VK_CHECK(vkCreateDescriptorSetLayout(logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));
}

void PipelineGraphics::createDescriptorPool() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	auto& descriptorPools = shader.getDescriptorPools();

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.maxSets = 8192; // 16384;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();
	VK_CHECK(vkCreateDescriptorPool(logicalDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool));
}

void PipelineGraphics::createPipelineLayout() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	auto pushConstantRanges = shader.getPushConstantRanges();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
	VK_CHECK(vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));
}

void PipelineGraphics::createAttributes() {
	const auto& physicalDevice = Graphics::Get()->getPhysicalDevice();
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

	if (polygonMode == VK_POLYGON_MODE_LINE && !logicalDevice.getEnabledFeatures().fillModeNonSolid) {
		throw std::runtime_error("Cannot create graphics pipeline with line polygon mode when logical device does not support non solid fills.");
	}

	inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyState.topology = topology;
	inputAssemblyState.primitiveRestartEnable = VK_FALSE;

	rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationState.depthClampEnable = VK_FALSE;
	rasterizationState.rasterizerDiscardEnable = VK_FALSE;
	rasterizationState.polygonMode = polygonMode;
	rasterizationState.cullMode = cullMode;
	rasterizationState.frontFace = frontFace;
	rasterizationState.depthBiasEnable = depthBiasEnabled ? VK_TRUE : VK_FALSE;
	rasterizationState.depthBiasConstantFactor = depthBiasConstantFactor;
	rasterizationState.depthBiasClamp = depthBiasClamp;
	rasterizationState.depthBiasSlopeFactor = depthBiasSlopeFactor;
	rasterizationState.lineWidth = lineWidth;

    for (auto& blendAttachmentState : blendAttachmentStates) {
        blendAttachmentState = VkPipelineColorBlendAttachmentState();
        blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendAttachmentState.alphaBlendOp = VK_BLEND_OP_MAX;
        blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;

        if (transparencyEnabled) {
            blendAttachmentState.blendEnable = VK_TRUE;
            blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;

            if (blend == Blend::SrcAlphaOneMinusSrcAlpha) {
                blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            } else if (blend == Blend::ZeroSrcColor) {
                blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
                blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
            } else if (blend == Blend::OneZero) {
                blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
                blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            } else {
                blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
                blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            }
        } else {
            blendAttachmentState.blendEnable = VK_FALSE;
            blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        }
    }

	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.logicOpEnable = VK_FALSE;
	colorBlendState.logicOp = VK_LOGIC_OP_COPY;
	colorBlendState.attachmentCount = static_cast<uint32_t>(blendAttachmentStates.size());
	colorBlendState.pAttachments = blendAttachmentStates.data();
	colorBlendState.blendConstants[0] = 0.0f;
	colorBlendState.blendConstants[1] = 0.0f;
	colorBlendState.blendConstants[2] = 0.0f;
	colorBlendState.blendConstants[3] = 0.0f;

	depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilState.front = depthStencilState.back;
	depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

	switch (depth) {
	case Depth::None:
		depthStencilState.depthTestEnable = VK_FALSE;
		depthStencilState.depthWriteEnable = VK_FALSE;
		break;
	case Depth::Read:
		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = VK_FALSE;
		break;
	case Depth::Write:
		depthStencilState.depthTestEnable = VK_FALSE;
		depthStencilState.depthWriteEnable = VK_TRUE;
		break;
	case Depth::ReadWrite:
		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = VK_TRUE;
		break;
	}

	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	// TODO: Multisampled pipelines
	//auto renderStage = Graphics::Get()->getRenderStage(stage.first);
	bool multisampled = false; // renderStage->IsMultisampled(stage.second);

	multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.rasterizationSamples = multisampled ? physicalDevice.getMsaaSamples() : VK_SAMPLE_COUNT_1_BIT;
	multisampleState.sampleShadingEnable = VK_FALSE;

    dynamicStates.reserve(4);

    dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

    if (topology == VK_PRIMITIVE_TOPOLOGY_LINE_LIST || topology == VK_PRIMITIVE_TOPOLOGY_LINE_STRIP || topology == VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY || topology == VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY) {
        dynamicStates.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
    }

    if (depthBiasEnabled) {
        dynamicStates.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
    }

    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	tessellationState.patchControlPoints = 3;
}

void PipelineGraphics::createPipeline() {
    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();
    const auto& pipelineCache = Graphics::Get()->getPipelineCache();
	auto renderStage = Graphics::Get()->getRenderStage(stage.first);

	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	uint32_t lastAttribute = 0;

	for (const auto& vertexInput : vertexInputs) {
		for (const auto& binding : vertexInput.getBindingDescriptions())
			bindingDescriptions.push_back(binding);

		for (const auto& attribute : vertexInput.getAttributeDescriptions()) {
			/*bool shaderContains = false;

			for (const auto& [shaderAttributeName, shaderAttribute] : shader->GetAttributes()) {
				if (attribute.location + lastAttribute == shaderAttribute->GetLocation()) {
					shaderContains = true;
					break;
				}
			}

			if (!shaderContains)
				continue;*/

			auto& newAttribute = attributeDescriptions.emplace_back(attribute);
			newAttribute.location += lastAttribute;
		}

		if (!vertexInput.getAttributeDescriptions().empty())
			lastAttribute = attributeDescriptions.back().location + 1;
	}

	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = static_cast<uint32_t>(stages.size());
	pipelineCreateInfo.pStages = stages.data();

	pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
	pipelineCreateInfo.pTessellationState = &tessellationState;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pRasterizationState = &rasterizationState;
	pipelineCreateInfo.pMultisampleState = &multisampleState;
	pipelineCreateInfo.pDepthStencilState = &depthStencilState;
	pipelineCreateInfo.pColorBlendState = &colorBlendState;
	pipelineCreateInfo.pDynamicState = &dynamicState;

	pipelineCreateInfo.layout = pipelineLayout;
	pipelineCreateInfo.renderPass = *renderStage->getRenderpass();
	pipelineCreateInfo.subpass = stage.second;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;
	VK_CHECK(vkCreateGraphicsPipelines(logicalDevice, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline));
}

void PipelineGraphics::createPipelinePolygon() {
	createPipeline();
}

void PipelineGraphics::createPipelineMrt() {
	auto renderStage = Graphics::Get()->getRenderStage(stage.first);
	auto attachmentCount = renderStage->getAttachmentCount(stage.second);

	std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentStates;
	blendAttachmentStates.reserve(attachmentCount);

	for (uint32_t i = 0; i < attachmentCount; i++) {
		VkPipelineColorBlendAttachmentState blendAttachmentState = {};
		blendAttachmentState.blendEnable = VK_TRUE;
		blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendAttachmentStates.push_back(blendAttachmentState);
	}

	colorBlendState.attachmentCount = static_cast<uint32_t>(blendAttachmentStates.size());
	colorBlendState.pAttachments = blendAttachmentStates.data();

	createPipeline();
}

void PipelineGraphics::bindPipeline(const CommandBuffer& commandBuffer) const {
    Pipeline::bindPipeline(commandBuffer);

#if FUSION_PLATFORM_APPLE
    // Bug in moltenVK. Needs to happen after pipeline bound for now.
    if(depthBiasEnabled)
        vkCmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
#endif
}