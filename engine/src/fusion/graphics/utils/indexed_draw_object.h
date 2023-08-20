#pragma once

#include "fusion/graphics/graphics.h"
#include "fusion/graphics/buffers/buffer.h"
#include "fusion/graphics/commands/command_buffer.h"

namespace fe {
    template<typename T, typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
    class FUSION_API IndexedDrawObject {
    public:
        void cmdRender(const CommandBuffer& commandBuffer, gsl::span<const T> newVertices, gsl::span<const I> newIndices, float lineWidth = 0.0f) {
            if (newVertices.empty() || newIndices.empty())
                return;

            if (!vertexBuffer || (vertexCount != newVertices.size())) {
                if (vertexBuffer) {
                    removeQueue.push(std::move(vertexBuffer));
                }
                vertexBuffer = std::make_unique<Buffer>(sizeof(T) * newVertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
                vertexBuffer->map();
                vertexCount = newVertices.size();
            }

            if (!indexBuffer || (indexCount != newIndices.size())) {
                if (indexBuffer) {
                    removeQueue.push(std::move(indexBuffer));
                }
                indexBuffer = std::make_unique<Buffer>(sizeof(I) * newIndices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
                indexBuffer->map();
                vertexCount = newIndices.size();
            }

            // Upload data
            std::memcpy(vertexBuffer->getMappedMemory(), newVertices.data(), newVertices.size() * sizeof(T));
            std::memcpy(indexBuffer->getMappedMemory(), newIndices.data(), newIndices.size() * sizeof(I));

            // Flush to make writes visible to GPU
            vertexBuffer->flush();
            indexBuffer->flush();

            // Bind and draw current buffers
            VkBuffer vertexBuffers[1] = { *vertexBuffer };
            VkDeviceSize offsets[1] = { 0 };
            if (lineWidth > 0.0f)
                vkCmdSetLineWidth(commandBuffer, lineWidth);
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(commandBuffer, *indexBuffer, 0, getIndexType());
            vkCmdDrawIndexed(commandBuffer, vertexCount, 1, 0, 0, 0);

            // Make sure that removed not in use by a command buffer.
            while (removeQueue.size() > MAX_FRAMES_IN_FLIGHT * 2) {
                removeQueue.pop();
            }
        }

        static constexpr VkIndexType getIndexType() {
            if constexpr (sizeof(uint8_t) == sizeof(I)) {
                return VK_INDEX_TYPE_UINT8_EXT;
            } else if constexpr (sizeof(uint16_t) == sizeof(I)) {
                return VK_INDEX_TYPE_UINT16;
            } else if constexpr (sizeof(uint32_t) == sizeof(I)) {
                return VK_INDEX_TYPE_UINT32;
            } else {
                static_assert("Invalid type");
            }
        }

    private:
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        int32_t vertexCount{ 0 };
        int32_t indexCount{ 0 };
        std::queue<std::unique_ptr<Buffer>> removeQueue;
    };
}