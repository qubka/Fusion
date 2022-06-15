#include "grid_renderer.hpp"
#include "fusion/renderer/vkx/pipelines.hpp"
#include "fusion/renderer/renderer.hpp"
#include "fusion/geometry/frustum.hpp"

using namespace fe;

GridRenderer* GridRenderer::instance{ nullptr };

void GridRenderer::createDescriptorSets() {

}

void GridRenderer::createPipelineLayout() {
    std::array<vk::DescriptorSetLayout, 1> descriptorSetLayouts{ renderer.getGlobalDescriptorLayoutSet() };

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{ {}, static_cast<uint32_t>(descriptorSetLayouts.size()), descriptorSetLayouts.data(), 0, nullptr };

    pipelineLayout = context.device.createPipelineLayout(pipelineLayoutCreateInfo);
}

void GridRenderer::createPipeline() {
    vkx::pipelines::GraphicsPipelineBuilder pipelineBuilder{ context.device, pipelineLayout, renderer.getDrawRenderPass() };
    //pipelineBuilder.rasterizationState.cullMode = vk::CullModeFlagBits::eNone;

    // Enable blending
    auto& blendAttachmentState = pipelineBuilder.colorBlendState.blendAttachmentStates[0];
    blendAttachmentState.blendEnable = VK_TRUE;
    blendAttachmentState.colorWriteMask = vkx::util::fullColorWriteMask();
    blendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    blendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    blendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
    blendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    blendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eOne; // eZero before
    blendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;

    // Attribute descriptions

    // vk::Pipeline for the meshes (armadillo, bunny, etc.)
    // Load shaders
    pipelineBuilder.loadShader(getAssetPath() + "/shaders/grid/grid.vert.spv", vk::ShaderStageFlagBits::eVertex);
    pipelineBuilder.loadShader(getAssetPath() + "/shaders/grid/grid.frag.spv", vk::ShaderStageFlagBits::eFragment);
    pipeline = pipelineBuilder.create(context.pipelineCache);
}

void GridRenderer::begin() {
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

void GridRenderer::draw() {
    assert(commandBuffer && "cannot draw grid when pipeline not bind");

    commandBuffer->draw(6, 1, 0, 0);
}

void GridRenderer::end() {
    commandBuffer = nullptr;
}