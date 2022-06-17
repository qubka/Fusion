#include "mesh_renderer.hpp"
#include "fusion/renderer/vkx/pipelines.hpp"
#include "fusion/renderer/renderer.hpp"
#include "fusion/utils/image.hpp"

using namespace fe;

MeshRenderer* MeshRenderer::instance{ nullptr };

void MeshRenderer::createUniformBuffers() {
    stb::Image image{ getAssetPath() + "/models/hq/loggerhead_8bit_albedo2.png", 4 };

    diffuseTexture.loadFromFile(context, getAssetPath() + "/textures/metalplate01_rgba.ktx");
    //diffuseTexture.fromBuffer(context, image.pixels, image.width * image.height * image.channels, vk::Extent2D{ static_cast<uint32_t>(image.width), static_cast<uint32_t>(image.height) });
    //diffuseTexture.loadFromFile(context, getAssetPath() + "/models/hq/loggerhead_8bit_albedo2.png");
    //normalTexture.loadFromFile(context, getAssetPath() + "/models/hq/loggerhead_8bit_normal.dds");
    //specularTexture.loadFromFile(context, getAssetPath() + "/models/hq/loggerhead_8bit_roughness.dds");

    lightsBuffer = context.createUniformBuffer(Lights{});
    materialBuffer = context.createUniformBuffer(Material{});
    parametersBuffer = context.createUniformBuffer(Parameters{});
}

void MeshRenderer::createDescriptorSets() {
    vkx::DescriptorBuilder{renderer.getDescriptorLayoutCache(), renderer.getGlobalAllocator()}
        .bindBuffer(0, &lightsBuffer.descriptor, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex)
        .bindBuffer(1, &materialBuffer.descriptor, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment)
        .bindBuffer(2, &parametersBuffer.descriptor, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment)
        .build(uniformSet, uniformDescriptorSetLayout);


    vkx::DescriptorBuilder{renderer.getDescriptorLayoutCache(), renderer.getGlobalAllocator()}
        .bindImage(0, &diffuseTexture.descriptor, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
        .bindImage(1, &diffuseTexture.descriptor, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
        .bindImage(2, &diffuseTexture.descriptor, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
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
    pipelineBuilder.rasterizationState.frontFace = vk::FrontFace::eCounterClockwise;

    // Binding description
    pipelineBuilder.vertexInputState.appendVertexLayout(vkx::Model::defaultLayout, 0, vk::VertexInputRate::eVertex);

    // Attribute descriptions

    // vk::Pipeline for the meshes (armadillo, bunny, etc.)
    // Load shaders
    pipelineBuilder.loadShader(getAssetPath() + "/shaders/mesh/blinn_phong.vert.spv", vk::ShaderStageFlagBits::eVertex);
    pipelineBuilder.loadShader(getAssetPath() + "/shaders/mesh/blinn_phong.frag.spv", vk::ShaderStageFlagBits::eFragment);
    pipeline = pipelineBuilder.create(context.pipelineCache);
}

void MeshRenderer::begin() {
    assert(!commandBuffer && "pipeline already was bind");

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
    assert(std::filesystem::exists(filename));
    if (auto it{ models.find(filename) }; it != models.end()) {
        return &it->second;
    }
    auto [it, result] = models.emplace(filename, vkx::Model{});
    auto& model = it->second;
    model.loadFromFile(context, filename);
    return &model;
}
