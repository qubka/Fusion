#include "mesh_renderer.hpp"
#include "fusion/renderer/vkx/pipelines.hpp"
#include "fusion/renderer/renderer.hpp"

using namespace fe;

MeshRenderer* MeshRenderer::instance{ nullptr };

void MeshRenderer::createDescriptorSets() {

}

void MeshRenderer::createPipelineLayout() {
    std::array<vk::DescriptorSetLayout, 1> descriptorSetLayouts{ renderer.getGlobalDescriptorLayoutSet() };

    vk::PushConstantRange pushConstantRange{ vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstantData) };
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{ {}, static_cast<uint32_t>(descriptorSetLayouts.size()), descriptorSetLayouts.data(), 1, &pushConstantRange };

    pipelineLayout = context.device.createPipelineLayout(pipelineLayoutCreateInfo);
}

void MeshRenderer::createPipeline() {
    vkx::pipelines::GraphicsPipelineBuilder pipelineBuilder{ context.device, pipelineLayout, renderer.getDrawRenderPass() };
    pipelineBuilder.rasterizationState.frontFace = vk::FrontFace::eClockwise;

    // Binding description
    pipelineBuilder.vertexInputState.appendVertexLayout(vkx::Model::defaultLayout, 0, vk::VertexInputRate::eVertex);

    // Attribute descriptions

    // vk::Pipeline for the meshes (armadillo, bunny, etc.)
    // Load shaders
    pipelineBuilder.loadShader(getAssetPath() + "/shaders/vulkanscene/mesh.vert.spv", vk::ShaderStageFlagBits::eVertex);
    pipelineBuilder.loadShader(getAssetPath() + "/shaders/vulkanscene/mesh.frag.spv", vk::ShaderStageFlagBits::eFragment);
    pipeline = pipelineBuilder.create(context.pipelineCache);
}

void MeshRenderer::begin() {
    assert(!commandBuffer && "pipeline already was bind");

    commandBuffer = &renderer.getCurrentCommandBuffer();

    std::array<vk::DescriptorSet, 1> descriptorSets{ renderer.getCurrentGlobalDescriptorSets() };

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
    if (auto it{ models.find(filename) }; it != models.end()) {
        return &it->second;
    }
    auto [it, result] = models.emplace(filename, vkx::Model{});
    auto& model = it->second;
    model.loadFromFile(context, filename);
    return &model;
}
