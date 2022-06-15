#include "sky_renderer.hpp"
#include "fusion/renderer/vkx/pipelines.hpp"
#include "fusion/renderer/renderer.hpp"
#include "fusion/core/application.hpp"

using namespace fe;

SkyRenderer* SkyRenderer::instance{ nullptr };

void SkyRenderer::createDescriptorSets() {

}

void SkyRenderer::createPipelineLayout() {
    std::array<vk::DescriptorSetLayout, 1> descriptorSetLayouts{ renderer.getGlobalDescriptorLayoutSet() };

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{ {}, static_cast<uint32_t>(descriptorSetLayouts.size()), descriptorSetLayouts.data(), 0, nullptr };

    pipelineLayout = context.device.createPipelineLayout(pipelineLayoutCreateInfo);
}

void SkyRenderer::createPipeline() {
    vkx::pipelines::GraphicsPipelineBuilder pipelineBuilder{ context.device, pipelineLayout, renderer.getDrawRenderPass() };
    //pipelineBuilder.rasterizationState.frontFace = vk::FrontFace::eCounterClockwise;
    //pipelineBuilder.rasterizationState.cullMode = vk::CullModeFlagBits::eNone;

    // Enable blending
    auto& blendAttachmentState = pipelineBuilder.colorBlendState.blendAttachmentStates[0];
    blendAttachmentState.blendEnable = VK_TRUE;
    blendAttachmentState.colorWriteMask = vkx::util::fullColorWriteMask();
    blendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    blendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    blendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
    blendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    blendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    blendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;

    // Attribute descriptions

    // vk::Pipeline for the meshes (armadillo, bunny, etc.)
    // Load shaders
    pipelineBuilder.loadShader(getAssetPath() + "/shaders/sky/sky.frag.spv", vk::ShaderStageFlagBits::eFragment);
    pipelineBuilder.loadShader(getAssetPath() + "/shaders/sky/sky.vert.spv", vk::ShaderStageFlagBits::eVertex);
    pipeline = pipelineBuilder.create(context.pipelineCache);
}

void SkyRenderer::begin() {
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

void SkyRenderer::draw() {
    assert(commandBuffer && "cannot draw grid when pipeline not bind");

    //PushConstantData push { { Application::Instance().getWindow()->getSize(), Application::Instance().getMouseInput().mousePosition() }, 0 };

    //commandBuffer->pushConstants<PushConstantData>(pipelineLayout, vk::ShaderStageFlagBits::eFragment, 0, push);
    commandBuffer->draw(6, 1, 0, 0);
}

void SkyRenderer::end() {
    commandBuffer = nullptr;
}