#include "grid_renderer.hpp"

#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/devices/devices.hpp"

using namespace fe;

GridRenderer::GridRenderer(const Pipeline::Stage& pipelineStage)
    : Subrender{pipelineStage}
    , pipeline{pipelineStage, {"shaders/grid/grid.vert", "shaders/grid/grid.frag"}, {GetVertexInput()}} {

    std::array<glm::vec2, 6> vertices{{
        {-1, 1}, {-1, -1}, {1, 1},
        {1, -1}, {1, 1}, {-1, -1}
    }};

    Buffer vertexStaging(sizeof(glm::vec2) * vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.data());
    vertexBuffer = std::make_unique<Buffer>(vertexStaging.getSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    CommandBuffer commandBuffer{true};

    VkBufferCopy copyRegion = {};
    copyRegion.size = vertexStaging.getSize();
    vkCmdCopyBuffer(commandBuffer, vertexStaging, *vertexBuffer, 1, &copyRegion);

    commandBuffer.submitIdle();
}

GridRenderer::~GridRenderer() {

}

void GridRenderer::render(const CommandBuffer& commandBuffer, float dt) {
    camera.update(dt);
    camera.setViewport(Devices::Get()->getWindow(0)->getSize());

    // Updates uniform
    //auto camera = Scenes::Get()->getScene()->getCamera();
    //uniformObject.push("projection", camera.getProjection());
    //uniformObject.push("view", camera.getView());
    pushObject.push("projection", camera.getProjection());
    pushObject.push("view", camera.getView());

    // Updates descriptors
    //descriptorSet.push("UniformObject", uniformObject);
    descriptorSet.push("PushObject", pushObject);

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushObject.bindPush(commandBuffer, pipeline);

    VkBuffer veryexBuffers[1] = { *vertexBuffer };
    VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(vertexBuffer->getBuffer()), offsets);
    vkCmdDraw(commandBuffer, 6, 1, 0, 0);
}

Shader::VertexInput GridRenderer::GetVertexInput(uint32_t baseBinding) {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
            {baseBinding, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX}
    };
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
            {0, baseBinding, VK_FORMAT_R32G32_SFLOAT, 0},
    };
    return {bindingDescriptions, attributeDescriptions};
}
