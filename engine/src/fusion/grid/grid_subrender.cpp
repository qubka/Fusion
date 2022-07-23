#include "grid_subrender.hpp"

#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/graphics/cameras/camera.hpp"
#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/devices/device_manager.hpp"
#include "fusion/scene/scene_manager.hpp"

using namespace fe;

GridSubrender::GridSubrender(const Pipeline::Stage& stage)
        : Subrender{stage}
        , pipeline{stage, {"EngineShaders/grid/grid.vert", "EngineShaders/grid/grid.frag"}, {{{Vertex::Component::Position2}}}} {

    std::array<glm::vec2, 6> vertices{{
        {-1, 1}, {-1, -1}, {1, 1},
        {1, -1}, {1, 1}, {-1, -1}
    }};

    vertexBuffer = Buffer::StageToDeviceBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, sizeof(glm::vec2) * vertices.size(), vertices.data());
}

GridSubrender::~GridSubrender() {
}

void GridSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    if (!camera)
        return;

    // Updates uniform
    pushObject.push("projection", camera->getProjectionMatrix());
    pushObject.push("view", camera->getViewMatrix());

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