#include "mesh_renderer.hpp"
#include "fusion/renderer/vkx/pipelines.hpp"
#include "fusion/renderer/renderer.hpp"

using namespace fe;

MeshRenderer* MeshRenderer::instance{ nullptr };

struct PushConstantData {
    glm::mat4 model{ 1.0f };
    glm::mat4 normal{ 1.0f };
};

struct ShadingUniforms {
    struct {
        glm::vec4 direction{ 0.1f, 0.5f, 0.3f, 0.5f };
        glm::vec4 radiance{ 0.5f };
    } lights[1];
    glm::vec4 eyePosition{ 100.0f };
};

void MeshRenderer::setup() {

}

void MeshRenderer::createUniformBuffers() {
    albedoTexture.fromFile(context, getAssetPath() + "/textures/cerberus_A.png", vk::Format::eR8G8B8A8Srgb);
    normalTexture.fromFile(context, getAssetPath() + "/textures/cerberus_N.png", vk::Format::eR8G8B8A8Unorm);
    metalnessTexture.fromFile(context, getAssetPath() + "/textures/cerberus_M.png", vk::Format::eR8Unorm);
    roughnessTexture.fromFile(context, getAssetPath() + "/textures/cerberus_R.png", vk::Format::eR8Unorm);

    static constexpr uint32_t kEnvMapSize = 1024;
    static constexpr uint32_t kIrradianceMapSize = 32;
    static constexpr uint32_t kBRDF_LUT_Size = 256;
    static constexpr uint32_t kEnvMapLevels = vkx::util::numMipmapLevels(kEnvMapSize, kEnvMapSize);
    static constexpr VkDeviceSize kUniformBufferSize = 64 * 1024;

    // Environment map (with pre-filtered mip chain)
    //envTexture.fromBuffer(context, nullptr, 0, { kEnvMapSize, kEnvMapSize }, 6, 0, vk::Format::eR16G16B16A16Sfloat, vk::Filter::eLinear, vk::ImageUsageFlagBits::eStorage);
    // Irradiance map
    //irmapTexture.fromBuffer(context, nullptr, 0, { kIrradianceMapSize, kIrradianceMapSize }, 6, 1, vk::Format::eR16G16B16A16Sfloat, vk::Filter::eLinear, vk::ImageUsageFlagBits::eStorage);
    // 2D LUT for split-sum approximation
    //spBRDF_LUT.fromBuffer(context, nullptr, 0, { kBRDF_LUT_Size, kBRDF_LUT_Size }, 1, 1, vk::Format::eR16G16Sfloat, vk::Filter::eLinear, vk::ImageUsageFlagBits::eStorage);

    uniformBuffer = context.createUniformBuffer(ShadingUniforms{});
}

void MeshRenderer::createDescriptorSets() {
    vkx::DescriptorBuilder{renderer.getDescriptorLayoutCache(), renderer.getGlobalAllocator()}
            .bindBuffer(0, &uniformBuffer.descriptor, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment)
            .build(uniformSet, uniformDescriptorSetLayout);

    vkx::DescriptorBuilder{renderer.getDescriptorLayoutCache(), renderer.getGlobalAllocator()}
        .bindImage(0, &albedoTexture.descriptor, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
        .bindImage(1, &normalTexture.descriptor, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
        .bindImage(2, &metalnessTexture.descriptor, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
        .bindImage(3, &roughnessTexture.descriptor, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
        /*.bindImage(4, &envTexture.descriptor, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
        .bindImage(5, &irmapTexture.descriptor, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
        .bindImage(6, &spBRDF_LUT.descriptor, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)*/
        .build(textureSet, textureDescriptorSetLayout);
}

void MeshRenderer::createPipelineLayout() {
    std::array<vk::DescriptorSetLayout, 3> descriptorSetLayouts{
            renderer.getGlobalDescriptorLayoutSet(),
            uniformDescriptorSetLayout,
            textureDescriptorSetLayout
    };

    vk::PushConstantRange pushConstantRange{ vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstantData) };
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{ {}, static_cast<uint32_t>(descriptorSetLayouts.size()), descriptorSetLayouts.data(), 1, &pushConstantRange };

    pipelineLayout = context.device.createPipelineLayout(pipelineLayoutCreateInfo);
}

void MeshRenderer::createPipeline() {
    vkx::pipelines::GraphicsPipelineBuilder pipelineBuilder{ context.device, pipelineLayout, renderer.getDrawRenderPass() };
    //pipelineBuilder.rasterizationState.frontFace = vk::FrontFace::eCounterClockwise;

    // Binding description
    pipelineBuilder.vertexInputState.appendVertexLayout(vkx::Model::defaultLayout, 0, vk::VertexInputRate::eVertex);

    // Attribute descriptions

    // vk::Pipeline for the meshes (armadillo, bunny, etc.)
    // Load shaders
    pipelineBuilder.loadShader(getAssetPath() + "/shaders/mesh/pbr.vert.spv", vk::ShaderStageFlagBits::eVertex);
    pipelineBuilder.loadShader(getAssetPath() + "/shaders/mesh/pbr.frag.spv", vk::ShaderStageFlagBits::eFragment);
    pipeline = pipelineBuilder.create(context.pipelineCache);
}

void MeshRenderer::begin() {
    assert(!commandBuffer && "pipeline already was bind");

    // update
    /*ShadingUniforms ubo{};
    ubo.eyePosition = ca;
    //ubo.cameraMatrix = editorCamera.getTransform();
    //ubo.frameTime = 0.0f;*/

    commandBuffer = &renderer.getCurrentCommandBuffer();

    std::array<vk::DescriptorSet, 3> descriptorSets{ renderer.getCurrentGlobalDescriptorSets(), uniformSet, textureSet };

    commandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    commandBuffer->bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipelineLayout,
            0,
            static_cast<uint32_t>(descriptorSets.size()),
            descriptorSets.data(),
            0,
            nullptr);
}

void MeshRenderer::draw(const vkx::Model& model, glm::mat4 transform) {
    assert(commandBuffer && "cannot draw mesh when pipeline not bind");

    // Invert Y-Scale for vulkan flipped offscreen
    if (renderer.getOffscreen().active)
        transform[1] *= -1.0f;
    PushConstantData push { transform, glm::transpose(glm::inverse(glm::mat3{transform})) };

    commandBuffer->pushConstants<PushConstantData>(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, push);
    commandBuffer->bindVertexBuffers(0, model.vertices.buffer, { 0 });
    if (model.indexCount) {
        commandBuffer->bindIndexBuffer(model.indices.buffer, 0, vk::IndexType::eUint32);
        commandBuffer->drawIndexed(model.indexCount, 1, 0, 0, 0);
    } else {
        commandBuffer->draw(model.vertexCount, 1, 0, 0);
    }
}

void MeshRenderer::end() {
    commandBuffer = nullptr;
}

vkx::Model* MeshRenderer::loadModel(const std::string& filename) {
    assert(std::fs::exists(filename));
    if (auto it{ models.find(filename) }; it != models.end()) {
        return &it->second;
    }
    auto [it, result] = models.emplace(filename, vkx::Model{});
    auto& model = it->second;
    model.fromFile(context, filename);
    return &model;
}
