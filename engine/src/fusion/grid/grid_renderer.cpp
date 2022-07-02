#include "grid_renderer.hpp"

#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/devices/devices.hpp"

using namespace fe;

GridRenderer::GridRenderer(const Pipeline::Stage& pipelineStage)
    : Subrender{pipelineStage}
    , pipeline{pipelineStage, {"shaders/grid/grid.vert", "shaders/grid/grid.frag"}, {{{Vertex::Component::Position2}}}} {

    std::array<glm::vec2, 6> vertices{{
        {-1, 1}, {-1, -1}, {1, 1},
        {1, -1}, {1, 1}, {-1, -1}
    }};

    Buffer vertexStaging{sizeof(glm::vec2) * vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.data()};
    vertexBuffer = std::make_unique<Buffer>(vertexStaging.getSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    CommandBuffer commandBuffer;

    VkBufferCopy copyRegion = {};
    copyRegion.size = vertexStaging.getSize();
    vkCmdCopyBuffer(commandBuffer, vertexStaging, *vertexBuffer, 1, &copyRegion);

    commandBuffer.submitIdle();
}

GridRenderer::~GridRenderer() {
}

void GridRenderer::render(const CommandBuffer& commandBuffer) {
    camera.update();

    auto& size = Devices::Get()->getWindow(0)->getSize();
    camera.setAspectRatio(static_cast<float>(size.x) / static_cast<float>(size.y));

    // Updates uniform
    //auto camera = Scenes::Get()->getScene()->getCamera();
    pushObject.push("projection", camera.getProjectionMatrix());
    pushObject.push("view", camera.getViewMatrix());

    // Updates descriptors
    descriptorSet.push("PushObject", pushObject);

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushObject.bindPush(commandBuffer, pipeline);

    VkBuffer vertexBuffers[1] = { *vertexBuffer };
    VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdDraw(commandBuffer, 6, 1, 0, 0);
}