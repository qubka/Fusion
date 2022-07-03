#include "mesh.hpp"

using namespace fe;

bool Mesh::render(const CommandBuffer& commandBuffer, uint32_t instances) {
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

std::vector<uint32_t> Mesh::getIndices() {
    auto indexStaging = Buffer::DeviceToStageBuffer(*indexBuffer);

    indexStaging->map();
    std::vector<uint32_t> indices(indexCount);
    indexStaging->extract(indices.data());
    indexStaging->unmap();

    return indices;
}

void Mesh::setIndices(const std::vector<uint32_t>& indices) {
    indexBuffer = nullptr;
    indexCount = static_cast<uint32_t>(indices.size());

    if (indices.empty())
        return;

    indexBuffer = Buffer::StageToDeviceBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, sizeof(uint32_t) * indices.size(), indices.data());
}
