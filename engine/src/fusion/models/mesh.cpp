#include "mesh.h"

using namespace fe;

Mesh::Mesh(uint32_t index) : index{index} {
}

bool Mesh::cmdRender(const CommandBuffer& commandBuffer, uint32_t instances) const {
    if (vertexBuffer && indexBuffer) {
        VkBuffer vertexBuffers[1] = { *vertexBuffer };
        VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, *indexBuffer, 0, indexType);
        vkCmdDrawIndexed(commandBuffer, indexCount, instances, 0, 0, 0);
    } else if (vertexBuffer && !indexBuffer) {
        VkBuffer vertexBuffers[1] = { *vertexBuffer };
        VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdDraw(commandBuffer, vertexCount, instances, 0, 0);
    } else {
        LOG_WARNING << "Mesh with no buffers can't be rendered";
        return false;
    }
    return true;
}