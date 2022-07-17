#include "shader.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/buffers/uniform_buffer.hpp"
#include "fusion/graphics/buffers/storage_buffer.hpp"
#include "fusion/graphics/images/image.hpp"
#include "fusion/graphics/images/image2d.hpp"
#include "fusion/graphics/images/image_cube.hpp"
#include "fusion/utils/string.hpp"
#include "fusion/filesystem/file_system.hpp"

#include <SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>

using namespace fe;

class ShaderIncluder : public glslang::TShader::Includer {
public:
	IncludeResult* includeLocal(const char* headerName, const char* includerName, size_t inclusionDepth) override {
		auto directory = fs::path(includerName).parent_path();
		auto fileLoaded = FileSystem::ReadText(directory / headerName);
		if (fileLoaded.empty()) {
            LOG_ERROR << "Shader Include could not be loaded: " << headerName;
			return nullptr;
		}

        auto size = fileLoaded.length() + 1;
        auto content = new char[size];
        std::strcpy(content, fileLoaded.c_str());
		return new IncludeResult(headerName, content, size, content);
	}

	IncludeResult* includeSystem(const char* headerName, const char* includerName, size_t inclusionDepth) override {
		auto fileLoaded = FileSystem::ReadText(headerName);
		if (fileLoaded.empty()) {
            LOG_ERROR << "Shader Include could not be loaded: " << headerName;
			return nullptr;
		}

        auto size = fileLoaded.length() + 1;
        auto content = new char[size];
        std::strcpy(content, fileLoaded.c_str());
		return new IncludeResult(headerName, content, size, content);
	}

	void releaseInclude(IncludeResult* result) override {
		if (result) {
			delete[] static_cast<char*>(result->userData);
			delete result;
		}
	}
};

VkFormat Shader::GlTypeToVk(int32_t type) {
	switch (type) {
        case 0x1406: // GL_FLOAT
            return VK_FORMAT_R32_SFLOAT;
        case 0x8B50: // GL_FLOAT_VEC2
            return VK_FORMAT_R32G32_SFLOAT;
        case 0x8B51: // GL_FLOAT_VEC3
            return VK_FORMAT_R32G32B32_SFLOAT;
        case 0x8B52: // GL_FLOAT_VEC4
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case 0x1404: // GL_INT
            return VK_FORMAT_R32_SINT;
        case 0x8B53: // GL_INT_VEC2
            return VK_FORMAT_R32G32_SINT;
        case 0x8B54: // GL_INT_VEC3
            return VK_FORMAT_R32G32B32_SINT;
        case 0x8B55: // GL_INT_VEC4
            return VK_FORMAT_R32G32B32A32_SINT;
        case 0x1405: // GL_UNSIGNED_INT
            return VK_FORMAT_R32_SINT;
        case 0x8DC6: // GL_UNSIGNED_INT_VEC2
            return VK_FORMAT_R32G32_SINT;
        case 0x8DC7: // GL_UNSIGNED_INT_VEC3
            return VK_FORMAT_R32G32B32_SINT;
        case 0x8DC8: // GL_UNSIGNED_INT_VEC4
            return VK_FORMAT_R32G32B32A32_SINT;
        default:
            return VK_FORMAT_UNDEFINED;
	}
}

bool Shader::reportedNotFound(const std::string& name, bool reportIfFound) const {
    if (std::find(notFoundNames.begin(), notFoundNames.end(), name) == notFoundNames.end()) {
        if (reportIfFound) {
            notFoundNames.push_back(name);
        }
        return true;
    }
    return false;
}

std::optional<uint32_t> Shader::getDescriptorLocation(const std::string& name) const {
	if (auto it = descriptorLocations.find(name); it != descriptorLocations.end())
		return it->second;
	return std::nullopt;
}

std::optional<uint32_t> Shader::getDescriptorSize(const std::string& name) const {
	if (auto it = descriptorSizes.find(name); it != descriptorSizes.end())
		return it->second;
	return std::nullopt;
}

std::optional<Shader::Uniform> Shader::getUniform(const std::string& name) const {
	if (auto it = uniforms.find(name); it != uniforms.end())
		return it->second;
	return std::nullopt;
}

std::optional<Shader::UniformBlock> Shader::getUniformBlock(const std::string& name) const {
	if (auto it = uniformBlocks.find(name); it != uniformBlocks.end())
		return it->second;
	return std::nullopt;
}

std::optional<Shader::Attribute> Shader::getAttribute(const std::string& name) const {
	if (auto it = attributes.find(name); it != attributes.end())
		return it->second;
	return std::nullopt;
}

std::vector<VkPushConstantRange> Shader::getPushConstantRanges() const {
	std::vector<VkPushConstantRange> pushConstantRanges;

	uint32_t currentOffset = 0;

	for (const auto& [uniformBlockName, uniformBlock] : uniformBlocks) {
		if (uniformBlock.type != UniformBlock::Type::Push)
			continue;

		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = uniformBlock.stageFlags;
		pushConstantRange.offset = currentOffset;
		pushConstantRange.size = static_cast<uint32_t>(uniformBlock.size);
		pushConstantRanges.push_back(pushConstantRange);
		currentOffset += pushConstantRange.size;
	}

	return pushConstantRanges;
}

std::optional<VkDescriptorType> Shader::getDescriptorType(uint32_t location) const {
	if (auto it = descriptorTypes.find(location); it != descriptorTypes.end())
		return it->second;
	return std::nullopt;
}

VkShaderStageFlagBits Shader::GetShaderStage(const fs::path& filepath) {
	auto fileExt = FileSystem::GetExtension(filepath);
	if (fileExt == ".comp")
		return VK_SHADER_STAGE_COMPUTE_BIT;
	if (fileExt == ".vert")
		return VK_SHADER_STAGE_VERTEX_BIT;
	if (fileExt == ".tesc")
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	if (fileExt == ".tese")
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	if (fileExt == ".geom")
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	if (fileExt == ".frag")
		return VK_SHADER_STAGE_FRAGMENT_BIT;

	return VK_SHADER_STAGE_ALL;
}

EShLanguage getEshLanguage(VkShaderStageFlags stageFlag) {
	switch (stageFlag) {
        case VK_SHADER_STAGE_COMPUTE_BIT:
            return EShLangCompute;
        case VK_SHADER_STAGE_VERTEX_BIT:
            return EShLangVertex;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return EShLangTessControl;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return EShLangTessEvaluation;
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return EShLangGeometry;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return EShLangFragment;
        default:
            return EShLangCount;
	}
}

TBuiltInResource getResources() {
	TBuiltInResource resources = {};
	resources.maxLights = 32;
	resources.maxClipPlanes = 6;
	resources.maxTextureUnits = 32;
	resources.maxTextureCoords = 32;
	resources.maxVertexAttribs = 64;
	resources.maxVertexUniformComponents = 4096;
	resources.maxVaryingFloats = 64;
	resources.maxVertexTextureImageUnits = 32;
	resources.maxCombinedTextureImageUnits = 80;
	resources.maxTextureImageUnits = 32;
	resources.maxFragmentUniformComponents = 4096;
	resources.maxDrawBuffers = 32;
	resources.maxVertexUniformVectors = 128;
	resources.maxVaryingVectors = 8;
	resources.maxFragmentUniformVectors = 16;
	resources.maxVertexOutputVectors = 16;
	resources.maxFragmentInputVectors = 15;
	resources.minProgramTexelOffset = -8;
	resources.maxProgramTexelOffset = 7;
	resources.maxClipDistances = 8;
	resources.maxComputeWorkGroupCountX = 65535;
	resources.maxComputeWorkGroupCountY = 65535;
	resources.maxComputeWorkGroupCountZ = 65535;
	resources.maxComputeWorkGroupSizeX = 1024;
	resources.maxComputeWorkGroupSizeY = 1024;
	resources.maxComputeWorkGroupSizeZ = 64;
	resources.maxComputeUniformComponents = 1024;
	resources.maxComputeTextureImageUnits = 16;
	resources.maxComputeImageUniforms = 8;
	resources.maxComputeAtomicCounters = 8;
	resources.maxComputeAtomicCounterBuffers = 1;
	resources.maxVaryingComponents = 60;
	resources.maxVertexOutputComponents = 64;
	resources.maxGeometryInputComponents = 64;
	resources.maxGeometryOutputComponents = 128;
	resources.maxFragmentInputComponents = 128;
	resources.maxImageUnits = 8;
	resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
	resources.maxCombinedShaderOutputResources = 8;
	resources.maxImageSamples = 0;
	resources.maxVertexImageUniforms = 0;
	resources.maxTessControlImageUniforms = 0;
	resources.maxTessEvaluationImageUniforms = 0;
	resources.maxGeometryImageUniforms = 0;
	resources.maxFragmentImageUniforms = 8;
	resources.maxCombinedImageUniforms = 8;
	resources.maxGeometryTextureImageUnits = 16;
	resources.maxGeometryOutputVertices = 256;
	resources.maxGeometryTotalOutputComponents = 1024;
	resources.maxGeometryUniformComponents = 1024;
	resources.maxGeometryVaryingComponents = 64;
	resources.maxTessControlInputComponents = 128;
	resources.maxTessControlOutputComponents = 128;
	resources.maxTessControlTextureImageUnits = 16;
	resources.maxTessControlUniformComponents = 1024;
	resources.maxTessControlTotalOutputComponents = 4096;
	resources.maxTessEvaluationInputComponents = 128;
	resources.maxTessEvaluationOutputComponents = 128;
	resources.maxTessEvaluationTextureImageUnits = 16;
	resources.maxTessEvaluationUniformComponents = 1024;
	resources.maxTessPatchComponents = 120;
	resources.maxPatchVertices = 32;
	resources.maxTessGenLevel = 64;
	resources.maxViewports = 16;
	resources.maxVertexAtomicCounters = 0;
	resources.maxTessControlAtomicCounters = 0;
	resources.maxTessEvaluationAtomicCounters = 0;
	resources.maxGeometryAtomicCounters = 0;
	resources.maxFragmentAtomicCounters = 8;
	resources.maxCombinedAtomicCounters = 8;
	resources.maxAtomicCounterBindings = 1;
	resources.maxVertexAtomicCounterBuffers = 0;
	resources.maxTessControlAtomicCounterBuffers = 0;
	resources.maxTessEvaluationAtomicCounterBuffers = 0;
	resources.maxGeometryAtomicCounterBuffers = 0;
	resources.maxFragmentAtomicCounterBuffers = 1;
	resources.maxCombinedAtomicCounterBuffers = 1;
	resources.maxAtomicCounterBufferSize = 16384;
	resources.maxTransformFeedbackBuffers = 4;
	resources.maxTransformFeedbackInterleavedComponents = 64;
	resources.maxCullDistances = 8;
	resources.maxCombinedClipAndCullDistances = 8;
	resources.maxSamples = 4;
	resources.limits.nonInductiveForLoops = true;
	resources.limits.whileLoops = true;
	resources.limits.doWhileLoops = true;
	resources.limits.generalUniformIndexing = true;
	resources.limits.generalAttributeMatrixVectorIndexing = true;
	resources.limits.generalVaryingIndexing = true;
	resources.limits.generalSamplerIndexing = true;
	resources.limits.generalVariableIndexing = true;
	resources.limits.generalConstantMatrixVectorIndexing = true;
	return resources;
}

VkShaderModule Shader::createShaderModule(const fs::path& moduleName, const std::string& moduleCode, const std::string& preamble, VkShaderStageFlags moduleFlag) {
	const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

    if (name.empty())
        name = moduleName.filename().replace_extension().string();

    stages.push_back(moduleName);

	// Starts converting GLSL to SPIR-V.
	auto language = getEshLanguage(moduleFlag);
	glslang::TProgram program;
	glslang::TShader shader(language);
	auto resources = getResources();

	// Enable SPIR-V and Vulkan rules when parsing GLSL.
	auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault);
#if FUSION_DEBUG
	messages = static_cast<EShMessages>(messages | EShMsgDebugInfo);
#endif

	auto shaderName = moduleName.string();
	auto shaderNameCstr = shaderName.c_str();
	auto shaderSource = moduleCode.c_str();
	shader.setStringsWithLengthsAndNames(&shaderSource, nullptr, &shaderNameCstr, 1);
	shader.setPreamble(preamble.c_str());

	auto defaultVersion = glslang::EShTargetVulkan_1_1;
	shader.setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClientVulkan, 110);
	shader.setEnvClient(glslang::EShClientVulkan, defaultVersion);
	shader.setEnvTarget(glslang::EShTargetSpv, volkGetInstanceVersion() >= VK_API_VERSION_1_1 ? glslang::EShTargetSpv_1_3 : glslang::EShTargetSpv_1_0);

	ShaderIncluder includer;

	std::string str;

	if (!shader.preprocess(&resources, defaultVersion, ENoProfile, false, false, messages, &str, includer)) {
        LOG_DEBUG << shader.getInfoLog();
        LOG_DEBUG << shader.getInfoDebugLog();
        LOG_ERROR << "SPRIV shader preprocess failed!";
	}

	if (!shader.parse(&resources, defaultVersion, true, messages, includer)) {
        LOG_DEBUG << shader.getInfoLog();
        LOG_DEBUG << shader.getInfoDebugLog();
        LOG_ERROR << "SPRIV shader parse failed!";
	}

	program.addShader(&shader);

	if (!program.link(messages) || !program.mapIO()) {
		LOG_ERROR << "Error while linking shader program.";
	}

	program.buildReflection();
	//program.dumpReflection();

	for (int32_t dim = 0; dim < 3; ++dim) {
		if (uint32_t localSize = program.getLocalSize(dim); localSize > 1)
			localSizes[dim] = localSize;
	}

	for (int32_t i = program.getNumLiveUniformBlocks() - 1; i >= 0; i--)
		loadUniformBlock(program, moduleFlag, i);

	for (int32_t i = 0; i < program.getNumLiveUniformVariables(); i++)
		loadUniform(program, moduleFlag, i);

	for (int32_t i = 0; i < program.getNumLiveAttributes(); i++)
		loadAttribute(program, moduleFlag, i);

	glslang::SpvOptions spvOptions;
#if FUSION_DEBUG
	spvOptions.generateDebugInfo = true;
	spvOptions.disableOptimizer = true;
	spvOptions.optimizeSize = false;
#else
	spvOptions.generateDebugInfo = false;
	spvOptions.disableOptimizer = false;
	spvOptions.optimizeSize = true;
#endif

	spv::SpvBuildLogger logger;
	std::vector<uint32_t> spirv;
	GlslangToSpv(*program.getIntermediate(static_cast<EShLanguage>(language)), spirv, &logger, &spvOptions);

	VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.codeSize = spirv.size() * sizeof(uint32_t);
	shaderModuleCreateInfo.pCode = spirv.data();

	VkShaderModule shaderModule;
	VK_CHECK(vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule));
	return shaderModule;
}

void Shader::createReflection() {
	std::unordered_map<VkDescriptorType, uint32_t> descriptorPoolCounts;

	// Process to descriptors.
	for (const auto& [uniformBlockName, uniformBlock] : uniformBlocks) {
		auto descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

		switch (uniformBlock.type) {
            case UniformBlock::Type::Uniform:
                descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorSetLayouts.push_back(UniformBuffer::GetDescriptorSetLayout(static_cast<uint32_t>(uniformBlock.binding), descriptorType, uniformBlock.stageFlags, 1));
                break;
            case UniformBlock::Type::Storage:
                descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                descriptorSetLayouts.push_back(StorageBuffer::GetDescriptorSetLayout(static_cast<uint32_t>(uniformBlock.binding), descriptorType, uniformBlock.stageFlags, 1));
                break;
            case UniformBlock::Type::Push:
                // Push constants are described in the pipeline.
                break;
            default:
                break;
		}

		incrementDescriptorPool(descriptorPoolCounts, descriptorType);
		descriptorLocations.emplace(uniformBlockName, uniformBlock.binding);
		descriptorSizes.emplace(uniformBlockName, uniformBlock.size);
	}

	for (const auto& [uniformName, uniform] : uniforms) {
		auto descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

		switch (uniform.glType) {
            case 0x8B5E: // GL_SAMPLER_2D
            case 0x904D: // GL_IMAGE_2D
            case 0x8DC1: // GL_TEXTURE_2D_ARRAY
            case 0x9108: // GL_SAMPLER_2D_MULTISAMPLE
            case 0x9055: // GL_IMAGE_2D_MULTISAMPLE
                descriptorType = uniform.writeOnly ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorSetLayouts.push_back(Image2d::GetDescriptorSetLayout(static_cast<uint32_t>(uniform.binding), descriptorType, uniform.stageFlags, 1));
                break;
            case 0x8B60: // GL_SAMPLER_CUBE
            case 0x9050: // GL_IMAGE_CUBE
            case 0x9054: // GL_IMAGE_CUBE_MAP_ARRAY
                descriptorType = uniform.writeOnly ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorSetLayouts.push_back(ImageCube::GetDescriptorSetLayout(static_cast<uint32_t>(uniform.binding), descriptorType, uniform.stageFlags, 1));
                break;
            default:
                break;
		}

		incrementDescriptorPool(descriptorPoolCounts, descriptorType);
		descriptorLocations.emplace(uniformName, uniform.binding);
		descriptorSizes.emplace(uniformName, uniform.size);
	}

	for (const auto& [type, descriptorCount] : descriptorPoolCounts) {
		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.type = type;
		descriptorPoolSize.descriptorCount = descriptorCount;
		descriptorPools.push_back(descriptorPoolSize);
	}

	// TODO: This is a AMD workaround that works on NVidia too...
	// We don't know the total usages of descriptor types by the pipeline.
	descriptorPools.resize(6);
	descriptorPools[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPools[0].descriptorCount = 4096;
	descriptorPools[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPools[1].descriptorCount = 2048;
	descriptorPools[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorPools[2].descriptorCount = 2048;
	descriptorPools[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	descriptorPools[3].descriptorCount = 2048;
	descriptorPools[4].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	descriptorPools[4].descriptorCount = 2048;
	descriptorPools[5].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorPools[5].descriptorCount = 2048;

	// Sort descriptors by binding.
	std::sort(descriptorSetLayouts.begin(), descriptorSetLayouts.end(), [](const VkDescriptorSetLayoutBinding& l, const VkDescriptorSetLayoutBinding& r) {
		return l.binding < r.binding;
	});

	// Gets the last descriptors binding.
	if (!descriptorSetLayouts.empty())
		lastDescriptorBinding = descriptorSetLayouts.back().binding;

	// Gets the descriptor type for each descriptor.
	for (const auto& descriptor : descriptorSetLayouts)
		descriptorTypes.emplace(descriptor.binding, descriptor.descriptorType);

	// Process attribute descriptions.
	uint32_t currentOffset = 4;

	for (const auto& [attributeName, attribute] : attributes) {
		VkVertexInputAttributeDescription attributeDescription = {};
		attributeDescription.location = static_cast<uint32_t>(attribute.location);
		attributeDescription.binding = 0;
		attributeDescription.format = GlTypeToVk(attribute.glType);
		attributeDescription.offset = currentOffset;
		attributeDescriptions.push_back(attributeDescription);
		currentOffset += attribute.size;
	}
}

void Shader::incrementDescriptorPool(std::unordered_map<VkDescriptorType, uint32_t>& descriptorPoolCounts, VkDescriptorType type) {
	if (type == VK_DESCRIPTOR_TYPE_MAX_ENUM)
		return;

	if (auto it = descriptorPoolCounts.find(type); it != descriptorPoolCounts.end())
		it->second++;
	else
		descriptorPoolCounts.emplace(type, 1);
}

void Shader::loadUniformBlock(const glslang::TProgram& program, VkShaderStageFlags stageFlag, int32_t i) {
	auto reflection = program.getUniformBlock(i);

    if (auto it = uniformBlocks.find(reflection.name); it != uniformBlocks.end()) {
        it->second.stageFlags |= stageFlag;
        return;
    }

	auto type = UniformBlock::Type::None;
	if (reflection.getType()->getQualifier().storage == glslang::EvqUniform)
		type = UniformBlock::Type::Uniform;
	if (reflection.getType()->getQualifier().storage == glslang::EvqBuffer)
		type = UniformBlock::Type::Storage;
	if (reflection.getType()->getQualifier().layoutPushConstant)
		type = UniformBlock::Type::Push;

	uniformBlocks.emplace(reflection.name, UniformBlock{ reflection.getBinding(), reflection.size, stageFlag, type });
}

void Shader::loadUniform(const glslang::TProgram& program, VkShaderStageFlags stageFlag, int32_t i) {
	auto reflection = program.getUniform(i);

	if (reflection.getBinding() == -1) {
		auto splitName = String::Split(reflection.name, '.');

		if (splitName.size() > 1) {
            auto& name = splitName.front();
            if (auto it = uniformBlocks.find(name); it != uniformBlocks.end()) {
                it->second.uniforms.emplace(String::ReplaceFirst(reflection.name, name + ".", ""),
                                            Uniform{ reflection.getBinding(), reflection.offset, computeSize(reflection.getType()), reflection.glDefineType, false, false, stageFlag });
                return;
            }
		}
	}

	for (auto& [uniformName, uniform] : uniforms) {
		if (uniformName == reflection.name) {
			uniform.stageFlags |= stageFlag;
			return;
		}
	}

	auto& qualifier = reflection.getType()->getQualifier();
	uniforms.emplace(reflection.name, Uniform{ reflection.getBinding(), reflection.offset, -1, reflection.glDefineType, qualifier.readonly, qualifier.writeonly, stageFlag });
}

void Shader::loadAttribute(const glslang::TProgram& program, VkShaderStageFlags stageFlag, int32_t i) {
	auto reflection = program.getPipeInput(i);

	if (reflection.name.empty())
		return;

    if (attributes.find(reflection.name) != attributes.end()) {
        return;
    }

	auto& qualifier = reflection.getType()->getQualifier();
	attributes.emplace(reflection.name, Attribute{ static_cast<int32_t>(qualifier.layoutSet), static_cast<int32_t>(qualifier.layoutLocation), computeSize(reflection.getType()), reflection.glDefineType });
}

int32_t Shader::computeSize(const glslang::TType* ttype) {
	// TODO: glslang::TType::computeNumComponents is available but has many issues resolved in this method.
	int32_t components = 0;

	if (ttype->getBasicType() == glslang::EbtStruct || ttype->getBasicType() == glslang::EbtBlock) {
		for (const auto& tl : *ttype->getStruct())
			components += computeSize(tl.type);
	} else if (ttype->getMatrixCols() != 0) {
		components = ttype->getMatrixCols() * ttype->getMatrixRows();
	} else {
		components = ttype->getVectorSize();
	}

	if (ttype->getArraySizes()) {
		int32_t arraySize = 1;

		for (int32_t d = 0; d < ttype->getArraySizes()->getNumDims(); ++d) {
			// This only makes sense in paths that have a known array size.
			if (auto dimSize = ttype->getArraySizes()->getDimSize(d); dimSize != glslang::UnsizedArraySize)
				arraySize *= dimSize;
		}

		components *= arraySize;
	}

	return sizeof(float) * components;
}