#pragma once

#include "fusion/graphics/graphics.h"
#include "fusion/graphics/buffers/buffer.h"
#include "fusion/graphics/commands/command_buffer.h"

namespace fe {
    template<typename T>
    class FUSION_API DrawObject {
    public:
        void cmdRender(const CommandBuffer& commandBuffer, gsl::span<const T> newVertices, float lineWidth = 0.0f) {
            if (newVertices.empty())
                return;

            if (!buffer || (vertexCount != newVertices.size())) {
                if (buffer) {
                    removeQueue.push(std::move(buffer));
                }
                buffer = std::make_unique<Buffer>(sizeof(T) * newVertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
                buffer->map();
                vertexCount = newVertices.size();
            }

            // Upload data
            std::memcpy(buffer->getMappedMemory(), newVertices.data(), newVertices.size() * sizeof(T));

            // Flush to make writes visible to GPU
            buffer->flush();

            // Bind and draw current buffers
            VkBuffer vertexBuffers[1] = { *buffer };
            VkDeviceSize offsets[1] = { 0 };
            if (lineWidth > 0.0f)
                vkCmdSetLineWidth(commandBuffer, lineWidth);
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);

            // Make sure that removed not in use by a command buffer.
            while (removeQueue.size() > MAX_FRAMES_IN_FLIGHT) {
                removeQueue.pop();
            }
        }

    private:
        std::unique_ptr<Buffer> buffer;
        uint32_t vertexCount{ 0 };
        std::queue<std::unique_ptr<Buffer>> removeQueue;
    };
}