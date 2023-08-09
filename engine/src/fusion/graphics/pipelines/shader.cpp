#include "shader.h"

#include "fusion/graphics/graphics.h"
#include "fusion/graphics/buffers/uniform_buffer.h"
#include "fusion/graphics/buffers/storage_buffer.h"
#include "fusion/graphics/textures/image.h"
#include "fusion/graphics/textures/texture2d.h"
#include "fusion/graphics/textures/texture_cube.h"
#include "fusion/filesystem/file_system.h"

#include <SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/MachineIndependent/gl_types.h>
#include <glslang/Include/BaseTypes.h>

using namespace fe;

class FUSION_API ShaderIncluder : public glslang::TShader::Includer {
public:
	IncludeResult* includeLocal(const char* headerName, const char* includerName, size_t inclusionDepth) override {
		auto directory = fs::path(includerName).parent_path();
		auto fileLoaded = FileSystem::ReadText(directory / headerName);
		if (fileLoaded.empty()) {
            FE_LOG_ERROR("Shader Include could not be loaded: '{}'", headerName);
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
            FE_LOG_ERROR("Shader Include could not be loaded: '{}'", headerName);
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

/**
 * Translate a glslang type into the GL API #define number. Ignores arrayness.
 */
int mapToGlType(const glslang::TType& type) {
    using namespace glslang;

    switch (type.getBasicType()) {
        case EbtSampler:
            //return mapSamplerToGlType(type.getSampler()); // TODO: We use mapToGlType only for specialization constant, so samplers not really important
        case EbtStruct:
        case EbtBlock:
        case EbtVoid:
            return 0;
        default:
            break;
    }

    if (type.isVector()) {
        int offset = type.getVectorSize() - 2;
        switch (type.getBasicType()) {
            case EbtFloat:      return GL_FLOAT_VEC2                  + offset;
            case EbtDouble:     return GL_DOUBLE_VEC2                 + offset;
            case EbtFloat16:    return GL_FLOAT16_VEC2_NV             + offset;
            case EbtInt:        return GL_INT_VEC2                    + offset;
            case EbtUint:       return GL_UNSIGNED_INT_VEC2           + offset;
            case EbtInt64:      return GL_INT64_VEC2_ARB              + offset;
            case EbtUint64:     return GL_UNSIGNED_INT64_VEC2_ARB     + offset;
            case EbtBool:       return GL_BOOL_VEC2                   + offset;
            case EbtAtomicUint: return GL_UNSIGNED_INT_ATOMIC_COUNTER + offset;
            default:            return 0;
        }
    }

    if (type.isMatrix()) {
        switch (type.getBasicType()) {
            case EbtFloat:
                switch (type.getMatrixCols()) {
                    case 2:
                        switch (type.getMatrixRows()) {
                            case 2:    return GL_FLOAT_MAT2;
                            case 3:    return GL_FLOAT_MAT2x3;
                            case 4:    return GL_FLOAT_MAT2x4;
                            default:   return 0;
                        }
                    case 3:
                        switch (type.getMatrixRows()) {
                            case 2:    return GL_FLOAT_MAT3x2;
                            case 3:    return GL_FLOAT_MAT3;
                            case 4:    return GL_FLOAT_MAT3x4;
                            default:   return 0;
                        }
                    case 4:
                        switch (type.getMatrixRows()) {
                            case 2:    return GL_FLOAT_MAT4x2;
                            case 3:    return GL_FLOAT_MAT4x3;
                            case 4:    return GL_FLOAT_MAT4;
                            default:   return 0;
                        }
                }
            case EbtDouble:
                switch (type.getMatrixCols()) {
                    case 2:
                        switch (type.getMatrixRows()) {
                            case 2:    return GL_DOUBLE_MAT2;
                            case 3:    return GL_DOUBLE_MAT2x3;
                            case 4:    return GL_DOUBLE_MAT2x4;
                            default:   return 0;
                        }
                    case 3:
                        switch (type.getMatrixRows()) {
                            case 2:    return GL_DOUBLE_MAT3x2;
                            case 3:    return GL_DOUBLE_MAT3;
                            case 4:    return GL_DOUBLE_MAT3x4;
                            default:   return 0;
                        }
                    case 4:
                        switch (type.getMatrixRows()) {
                            case 2:    return GL_DOUBLE_MAT4x2;
                            case 3:    return GL_DOUBLE_MAT4x3;
                            case 4:    return GL_DOUBLE_MAT4;
                            default:   return 0;
                        }
                }
            case EbtFloat16:
                switch (type.getMatrixCols()) {
                    case 2:
                        switch (type.getMatrixRows()) {
                            case 2:    return GL_FLOAT16_MAT2_AMD;
                            case 3:    return GL_FLOAT16_MAT2x3_AMD;
                            case 4:    return GL_FLOAT16_MAT2x4_AMD;
                            default:   return 0;
                        }
                    case 3:
                        switch (type.getMatrixRows()) {
                            case 2:    return GL_FLOAT16_MAT3x2_AMD;
                            case 3:    return GL_FLOAT16_MAT3_AMD;
                            case 4:    return GL_FLOAT16_MAT3x4_AMD;
                            default:   return 0;
                        }
                    case 4:
                        switch (type.getMatrixRows()) {
                            case 2:    return GL_FLOAT16_MAT4x2_AMD;
                            case 3:    return GL_FLOAT16_MAT4x3_AMD;
                            case 4:    return GL_FLOAT16_MAT4_AMD;
                            default:   return 0;
                        }
                }
            default:
                return 0;
        }
    }

    if (type.getVectorSize() == 1) {
        switch (type.getBasicType()) {
            case EbtFloat:      return GL_FLOAT;
            case EbtDouble:     return GL_DOUBLE;
            case EbtFloat16:    return GL_FLOAT16_NV;
            case EbtInt:        return GL_INT;
            case EbtUint:       return GL_UNSIGNED_INT;
            case EbtInt64:      return GL_INT64_ARB;
            case EbtUint64:     return GL_UNSIGNED_INT64_ARB;
            case EbtBool:       return GL_BOOL;
            case EbtAtomicUint: return GL_UNSIGNED_INT_ATOMIC_COUNTER;
            default:            return 0;
        }
    }

    return 0;
}

template <typename T>
bool ptrComp(const T* const & a, const T* const& b) {
    return *a < *b;
}

VkFormat Shader::GlTypeToVk(int32_t type) {
    switch (type) {
        case GL_BOOL:
            return VK_FORMAT_R32_UINT;
        case GL_FLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case GL_FLOAT_VEC2:
            return VK_FORMAT_R32G32_SFLOAT;
        case GL_FLOAT_VEC3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case GL_FLOAT_VEC4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case GL_INT:
            return VK_FORMAT_R32_SINT;
        case GL_INT_VEC2:
            return VK_FORMAT_R32G32_SINT;
        case GL_INT_VEC3:
            return VK_FORMAT_R32G32B32_SINT;
        case GL_INT_VEC4:
            return VK_FORMAT_R32G32B32A32_SINT;
        case GL_UNSIGNED_INT:
            return VK_FORMAT_R32_UINT;
        case GL_UNSIGNED_INT_VEC2:
            return VK_FORMAT_R32G32_UINT;
        case GL_UNSIGNED_INT_VEC3:
            return VK_FORMAT_R32G32B32_UINT;
        case GL_UNSIGNED_INT_VEC4:
            return VK_FORMAT_R32G32B32A32_UINT;
        default:
            return VK_FORMAT_UNDEFINED;
    }
}

uint32_t Shader::GlTypeToSize(int32_t type) {
    switch (type) {
        case GL_BOOL:
            return sizeof(VkBool32);
        case GL_FLOAT:
            return sizeof(float);
        case GL_FLOAT_VEC2:
            return sizeof(glm::vec2);
        case GL_FLOAT_VEC3:
            return sizeof(glm::vec3);
        case GL_FLOAT_VEC4:
            return sizeof(glm::vec4);
        case GL_INT:
            return sizeof(int32_t);
        case GL_INT_VEC2:
            return sizeof(glm::ivec2);
        case GL_INT_VEC3:
            return sizeof(glm::ivec3);
        case GL_INT_VEC4:
            return sizeof(glm::ivec4);
        case GL_UNSIGNED_INT:
            return sizeof(uint32_t);
        case GL_UNSIGNED_INT_VEC2:
            return sizeof(glm::uvec2);
        case GL_UNSIGNED_INT_VEC3:
            return sizeof(glm::uvec3);
        case GL_UNSIGNED_INT_VEC4:
            return sizeof(glm::uvec4);
        default:
            return VK_FORMAT_UNDEFINED;
    }
}

VkShaderStageFlagBits Shader::GetShaderStage(const fs::path& path) {
    std::string extension{ FileSystem::GetExtension(path) };
    if (extension == ".comp")
        return VK_SHADER_STAGE_COMPUTE_BIT;
    else if (extension == ".vert")
        return VK_SHADER_STAGE_VERTEX_BIT;
    else if (extension == ".tesc")
        return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    else if (extension == ".tese")
        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    else if (extension == ".geom")
        return VK_SHADER_STAGE_GEOMETRY_BIT;
    else if (extension == ".frag")
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    else
        return VK_SHADER_STAGE_ALL;
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

std::optional<VkDescriptorType> Shader::getDescriptorType(uint32_t location) const {
    if (auto it = descriptorTypes.find(location); it != descriptorTypes.end())
        return it->second;
    return std::nullopt;
}

std::optional<uint32_t> Shader::getDescriptorSet(const std::string& name) const {
    if (auto it = descriptorSets.find(name); it != descriptorSets.end())
        return it->second;
    return std::nullopt;
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

std::optional<Shader::Constant> Shader::getConstant(const std::string& name) const {
    if (auto it = constants.find(name); it != constants.end())
        return it->second;
    return std::nullopt;
}

std::vector<VkSpecializationMapEntry> Shader::getSpecializationMapEntries(VkShaderStageFlagBits moduleFlag) const {
    if (constants.empty())
        return {};

    std::vector<const Constant*> values;
    for (const auto& [constantName, constant] : constants) {
        if (constant.stageFlags == moduleFlag)
            values.push_back(&constant);
    }

    switch (values.size()) {
        case 0:
            return {};
        case 1:
            break;
        default:
            std::sort(values.begin(), values.end(), ptrComp<Constant>);
            break;
    }

    uint32_t currentOffset = 0;
    std::vector<VkSpecializationMapEntry> specializationMapEntries;
    specializationMapEntries.reserve(values.size());

    for (const auto constant : values) {
        auto& specializationMapEntry = specializationMapEntries.emplace_back();
        specializationMapEntry.constantID = static_cast<uint32_t>(constant->specId);
        specializationMapEntry.offset = currentOffset;
        specializationMapEntry.size = static_cast<uint32_t>(constant->size);
        currentOffset += specializationMapEntry.size;
    }

    return specializationMapEntries;
}

std::vector<VkPushConstantRange> Shader::getPushConstantRanges() const {
    if (uniformBlocks.empty())
        return {};

    std::vector<const UniformBlock*> values;
    for (const auto& [uniformName, uniformBlock] : uniformBlocks) {
        if (uniformBlock.type == UniformBlock::Type::Push)
            values.push_back(&uniformBlock);
    }

    switch (values.size()) {
        case 0:
            return {};
        case 1:
            break;
        default:
            std::sort(values.begin(), values.end(), ptrComp<UniformBlock>);
            break;
    }

    uint32_t currentOffset = 0;
    std::vector<VkPushConstantRange> pushConstantRanges;
    pushConstantRanges.reserve(values.size());

    for (const auto uniformBlock : values) {
        auto& pushConstantRange = pushConstantRanges.emplace_back();
        pushConstantRange.stageFlags = uniformBlock->stageFlags;
        pushConstantRange.offset = currentOffset;
        pushConstantRange.size = static_cast<uint32_t>(uniformBlock->size);
        currentOffset += pushConstantRange.size;
    }

    return pushConstantRanges;
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

VkShaderModule Shader::createShaderModule(const std::string& moduleName, const std::string& moduleCode, VkShaderStageFlagBits moduleFlag) {
	const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

    if (name.empty())
        name = String::Extract(moduleName, "", ".");

	// Starts converting GLSL to SPIR-V.
	auto language = getEshLanguage(moduleFlag);
	glslang::TProgram program;
	glslang::TShader shader{language};
	auto resources = getResources();

	// Enable SPIR-V and Vulkan rules when parsing GLSL.
	auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault);
#if FUSION_DEBUG
	messages = static_cast<EShMessages>(messages | EShMsgDebugInfo);
#endif

	auto shaderName = moduleName.c_str();
	auto shaderSource = moduleCode.c_str();
	shader.setStringsWithLengthsAndNames(&shaderSource, nullptr, &shaderName, 1);
	//shader.setPreamble(preamble.c_str());

	auto defaultVersion = glslang::EShTargetVulkan_1_3;
	shader.setEnvInput(glslang::EShSourceGlsl, language, glslang::EShClientVulkan, 110);
	shader.setEnvClient(glslang::EShClientVulkan, defaultVersion);
	shader.setEnvTarget(glslang::EShTargetSpv, volkGetInstanceVersion() >= VK_API_VERSION_1_1 ? glslang::EShTargetSpv_1_3 : glslang::EShTargetSpv_1_0);

	ShaderIncluder includer;

	std::string str;

	if (!shader.preprocess(&resources, defaultVersion, ENoProfile, false, false, messages, &str, includer)) {
        FE_LOG_DEBUG(shader.getInfoLog());
        FE_LOG_DEBUG(shader.getInfoDebugLog());
        FE_LOG_ERROR("SPRIV shader preprocess failed!");
	}

	if (!shader.parse(&resources, defaultVersion, true, messages, includer)) {
        FE_LOG_DEBUG(shader.getInfoLog());
        FE_LOG_DEBUG(shader.getInfoDebugLog());
        FE_LOG_ERROR("SPRIV shader parse failed!");
	}

	program.addShader(&shader);

	if (!program.link(messages) || !program.mapIO()) {
		FE_LOG_ERROR("Error while linking shader program.");
	}

	program.buildReflection();
	//program.dumpReflection();

    auto intermediate = program.getIntermediate(language);

	for (int32_t dim = 0; dim < 3; ++dim) {
		if (uint32_t localSize = program.getLocalSize(dim); localSize > 1)
			localSizes[dim] = localSize;
	}

	for (int32_t i = program.getNumLiveUniformBlocks() - 1; i >= 0; --i)
		loadUniformBlock(program, moduleFlag, i);

	for (int32_t i = 0; i < program.getNumLiveUniformVariables(); ++i)
		loadUniform(program, moduleFlag, i);

	for (int32_t i = 0; i < program.getNumLiveAttributes(); ++i)
		loadAttribute(program, moduleFlag, i);

    // Custom traverser used
    loadConstants(*intermediate, moduleFlag);

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
	GlslangToSpv(*intermediate, spirv, &logger, &spvOptions);

	VkShaderModuleCreateInfo shaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	shaderModuleCreateInfo.codeSize = spirv.size() * sizeof(uint32_t);
	shaderModuleCreateInfo.pCode = spirv.data();

	VkShaderModule shaderModule;
	VK_CHECK(vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule));
	return shaderModule;
}

std::optional<Shader::Specialization> Shader::createSpecialization(const fst::unordered_flatmap<std::string, Shader::SpecConstant>& specConstants, VkShaderStageFlagBits moduleFlag) const {
    auto mapEntries = getSpecializationMapEntries(moduleFlag);
    if (mapEntries.empty())
        return std::nullopt;

    fst::split_flatmap<uint32_t, SpecConstant> data;
    // Sorting constants by specId for specified stage
    for (const auto& [constantName, specConstant] : specConstants) {
        auto constant = getConstant(constantName);
        if (constant && constant->getStageFlags() & moduleFlag) {
            data.emplace(constant->getSpecId(), specConstant);
        }
    }

    if (data.empty())
        return std::nullopt;

    if (data.size() != mapEntries.size()) {
        FE_LOG_ERROR("Invalid amount of specialization constants provided in '{}' required = {}, provided = {}", name, mapEntries.size(), data.size());
        return std::nullopt;
    }

    Specialization specialization = {};
    specialization.mapEntries = std::move(mapEntries);
    specialization.data = std::move(data);
    specialization.info.mapEntryCount = static_cast<uint32_t>(specialization.mapEntries.size());
    specialization.info.pMapEntries = specialization.mapEntries.data();
    specialization.info.dataSize = static_cast<uint32_t>(specialization.data.size() * sizeof(SpecConstant));
    specialization.info.pData = specialization.data.values().data();
    return specialization;
}

void Shader::createReflection() {
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

        descriptorSets.emplace(uniformBlockName, uniformBlock.set);
		descriptorLocations.emplace(uniformBlockName, uniformBlock.binding);
		descriptorSizes.emplace(uniformBlockName, uniformBlock.size);
	}

	for (const auto& [uniformName, uniform] : uniforms) {
		auto descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

		switch (uniform.glType) {
            case GL_IMAGE_2D:
            case GL_SAMPLER_2D:
            case GL_SAMPLER_2D_ARRAY:
            case GL_SAMPLER_2D_MULTISAMPLE:
            case GL_IMAGE_2D_MULTISAMPLE:
                descriptorType = uniform.writeOnly ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorSetLayouts.push_back(Texture2d::GetDescriptorSetLayout(static_cast<uint32_t>(uniform.binding), descriptorType, uniform.stageFlags, uniform.size));
                break;
            case GL_SAMPLER_CUBE:
            case GL_IMAGE_CUBE:
            case GL_IMAGE_CUBE_MAP_ARRAY:
                descriptorType = uniform.writeOnly ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorSetLayouts.push_back(TextureCube::GetDescriptorSetLayout(static_cast<uint32_t>(uniform.binding), descriptorType, uniform.stageFlags, uniform.size));
                break;
            default:
                break;
		}

        descriptorSets.emplace(uniformName, uniform.set);
		descriptorLocations.emplace(uniformName, uniform.binding);
		descriptorSizes.emplace(uniformName, uniform.size);
	}

	// Sort descriptors by binding.
    std::sort(descriptorSetLayouts.begin(), descriptorSetLayouts.end(), [](const VkDescriptorSetLayoutBinding& l, const VkDescriptorSetLayoutBinding& r) {
        return l.binding < r.binding;
    });

	// Gets the descriptor type for each descriptor.
    for (const auto& layout : descriptorSetLayouts) {
        descriptorTypes.emplace(layout.binding, layout.descriptorType);
    }

    // Process attribute descriptions.
    {
        std::vector<const Attribute*> values;
        for (const auto& [attributeName, attribute] : attributes) {
            values.push_back(&attribute);
        }

        switch (values.size()) {
            case 0:
                return;
            case 1:
                break;
            default:
                std::sort(values.begin(), values.end(), ptrComp<Attribute>);
                break;
        }

        uint32_t currentOffset = 0;
        attributeDescriptions.reserve(values.size());

        for (const auto attribute : values) {
            auto& attributeDescription = attributeDescriptions.emplace_back();
            attributeDescription.location = static_cast<uint32_t>(attribute->location);
            attributeDescription.binding = 0;
            attributeDescription.format = GlTypeToVk(attribute->glType);
            attributeDescription.offset = currentOffset;
            currentOffset += attribute->size;
        }
    }
}

void Shader::loadUniformBlock(const glslang::TProgram& program, VkShaderStageFlagBits stageFlag, int32_t i) {
	auto reflection = program.getUniformBlock(i);
    if (reflection.name.empty())
        return;

    auto& qualifier = reflection.getType()->getQualifier();

    if (auto it = uniformBlocks.find(reflection.name); it != uniformBlocks.end()) {
        auto& uniformBlock = it->second;
        uniformBlock.stageFlags |= stageFlag;
        return;
    }

	auto type = UniformBlock::Type::None;
	if (qualifier.storage == glslang::EvqUniform)
		type = UniformBlock::Type::Uniform;
	if (qualifier.storage == glslang::EvqBuffer)
		type = UniformBlock::Type::Storage;
	if (qualifier.layoutPushConstant)
		type = UniformBlock::Type::Push;

	uniformBlocks.emplace(reflection.name, UniformBlock{ static_cast<int32_t>(qualifier.layoutSet), reflection.getBinding(), reflection.size, stageFlag, type });
}

void Shader::loadUniform(const glslang::TProgram& program, VkShaderStageFlagBits stageFlag, int32_t i) {
	auto reflection = program.getUniform(i);
    if (reflection.name.empty())
        return;

    auto& qualifier = reflection.getType()->getQualifier();

	if (reflection.getBinding() == -1) {
		auto splitName = String::Split(reflection.name, '.');

		if (splitName.size() > 1) {
            auto& name = splitName.front();
            if (auto it = uniformBlocks.find(name); it != uniformBlocks.end()) {
                auto& uniformBlock = it->second;
                uniformBlock.uniforms.emplace(String::ReplaceFirst(reflection.name, name + ".", ""),
                                              Uniform{ static_cast<int32_t>(qualifier.layoutSet), reflection.getBinding(), reflection.offset,
                                                       computeSize(*reflection.getType()), reflection.glDefineType, false, false, stageFlag });
                return;
            }
		}
	}

    if (auto it = uniforms.find(reflection.name); it != uniforms.end()) {
        auto& uniform = it->second;
        uniform.stageFlags |= stageFlag;
        return;
    }

	uniforms.emplace(reflection.name, Uniform{ static_cast<int32_t>(qualifier.layoutSet), reflection.getBinding(), reflection.offset,
                                               reflection.size, reflection.glDefineType, qualifier.readonly, qualifier.writeonly, stageFlag });
}

void Shader::loadAttribute(const glslang::TProgram& program, VkShaderStageFlagBits stageFlag, int32_t i) {
	auto reflection = program.getPipeInput(i);
	if (reflection.name.empty())
		return;

    if (attributes.find(reflection.name) != attributes.end())
        return;

	auto& qualifier = reflection.getType()->getQualifier();
	attributes.emplace(reflection.name, Attribute{ static_cast<int32_t>(qualifier.layoutSet), static_cast<int32_t>(qualifier.layoutLocation), computeSize(*reflection.getType()), reflection.glDefineType });
}

void Shader::loadConstants(const glslang::TIntermediate& intermediate, VkShaderStageFlagBits stageFlag) {
    class FUSION_API ConstantTraverser : public glslang::TIntermTraverser {
    public:
        ConstantTraverser(fst::unordered_flatmap<std::string, Shader::Constant>& constants, VkShaderStageFlagBits stageFlag)
                : TIntermTraverser{}, constants{constants}, stageFlag{stageFlag} {}

        void visitSymbol(glslang::TIntermSymbol* symbol) override {
            if (symbol->getQualifier().isSpecConstant()) {
                std::string name{ symbol->getName() };
                if (name.empty())
                    return;

                auto specId = static_cast<int32_t>(symbol->getQualifier().layoutSpecConstantId);

                if (auto it = constants.find(name); it != constants.end()) {
                    auto& constant = it->second;
                    if (constant.specId != specId) {
                        FE_LOG_WARNING("Same constants with different specialization constant Id");
                    }
                    constant.stageFlags |= stageFlag;
                    return;
                }

                constants.emplace(std::move(name), Constant{ specId, computeSize(symbol->getType()), stageFlag, mapToGlType(symbol->getType()) });
            }
        }

    private:
        fst::unordered_flatmap<std::string, Shader::Constant>& constants;
        VkShaderStageFlagBits stageFlag;
    };

    ConstantTraverser traverser{constants, stageFlag};
    auto root = intermediate.getTreeRoot();
    root->traverse(&traverser);
}

int32_t Shader::computeSize(const glslang::TType& type) {
	// TODO: glslang::TType::computeNumComponents is available but has many issues resolved in this method.
	int32_t components = 0;

	if (type.getBasicType() == glslang::EbtStruct || type.getBasicType() == glslang::EbtBlock) {
		for (const auto& tl : *type.getStruct())
			components += computeSize(*tl.type);
	} else if (type.getMatrixCols() != 0) {
		components = type.getMatrixCols() * type.getMatrixRows();
	} else {
		components = type.getVectorSize();
	}

	if (type.getArraySizes()) {
		int32_t arraySize = 1;

		for (int32_t d = 0; d < type.getArraySizes()->getNumDims(); ++d) {
			// This only makes sense in paths that have a known array size.
			if (auto dimSize = type.getArraySizes()->getDimSize(d); dimSize != glslang::UnsizedArraySize)
				arraySize *= dimSize;
		}

		components *= arraySize;
	}

	return sizeof(float) * components;
}