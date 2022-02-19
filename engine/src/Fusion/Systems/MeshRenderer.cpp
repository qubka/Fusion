#include "MeshRenderer.hpp"

#include "Fusion/Renderer/Pipeline.hpp"
#include "Fusion/Renderer/Mesh.hpp"
#include "Fusion/Renderer/Texture.hpp"
#include "Fusion/Renderer/Descriptors.hpp"
#include "Fusion/Renderer/SwapChain.hpp"
#include "Fusion/Renderer/Offscreen.hpp"

using namespace Fusion;

MeshRenderer::MeshRenderer(Vulkan& vulkan, Renderer& renderer) : vulkan{vulkan}, renderer{renderer} {
    createDescriptorSets();
    createPipelineLayout();
    createPipeline();
}

MeshRenderer::~MeshRenderer() {
    vulkan.getDevice().destroyPipelineLayout(pipelineLayout, nullptr);
}

void MeshRenderer::createDescriptorSets() {
    /*texture = std::make_unique<Texture>(vulkan, "assets/textures/texture.jpg", vk::Format::eR8G8B8A8Srgb);

    vk::DescriptorImageInfo imageInfo{};
    imageInfo.sampler = texture->getSampler();
    imageInfo.imageView = texture->getView();
    imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    DescriptorWriter(*textureLayout, *texturePool)
            .writeImage(0, imageInfo)
            .build(textureDescriptorSet);*/
}

void MeshRenderer::createPipelineLayout() {
    vk::PushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PushConstantData);

    std::array<vk::DescriptorSetLayout, 1> descriptorSetLayouts{renderer.getGlobalDescriptorLayoutSet()};

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    auto result = vulkan.getDevice().createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout);;
    FE_ASSERT(result == vk::Result::eSuccess && "failed to create pipeline layout!");
}

void MeshRenderer::createPipeline() {
    PipelineConfigInfo configInfo{};
    Pipeline::defaultPipelineConfigInfo(configInfo);
    configInfo.pipelineLayout = pipelineLayout;
    configInfo.renderPass = renderer.getOffscreen().getRenderPass();
    configInfo.subpass = 0;
    pipeline = std::make_unique<Pipeline>(vulkan, "assets/shaders/mesh.vert.spv", "assets/shaders/mesh.frag.spv", configInfo);
}

void MeshRenderer::beginScene() {
    FE_ASSERT(!commandBuffer && "pipeline already was bind");

    commandBuffer = &renderer.getCurrentCommandBuffer();

    pipeline->bind(*commandBuffer);

    std::array<vk::DescriptorSet, 1> descriptorSets { renderer.getCurrentGlobalDescriptorSets() };

    commandBuffer->bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipelineLayout,
            0,
            static_cast<uint32_t>(descriptorSets.size()),
            descriptorSets.data(),
            0,
            nullptr);
}

void MeshRenderer::drawMesh(const glm::mat4& transform, const std::shared_ptr<Mesh>& mesh) {
    FE_ASSERT(commandBuffer && "cannot draw mesh when pipeline not bind");

    PushConstantData push { transform, glm::transpose(glm::inverse(glm::mat3{transform})) };

    commandBuffer->pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstantData), &push);

    mesh->bind(*commandBuffer);
    mesh->draw(*commandBuffer);
}

void MeshRenderer::endScene() {
    commandBuffer = nullptr;
}
