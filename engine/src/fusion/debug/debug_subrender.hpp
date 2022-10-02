#pragma once

#include "debug_renderer.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"
#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"

namespace fe {
    class DebugSubrender final : public Subrender {
    public:
        explicit DebugSubrender(Pipeline::Stage pipelineStage);
        ~DebugSubrender() override;

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        template<typename T>
        class DrawBuffer {
        public:
            void cmdRender(const CommandBuffer& commandBuffer, const std::vector<T>& newVertices, float lineWidth = 0.0f) {
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

                std::memcpy(buffer->getMappedMemory(), newVertices.data(), newVertices.size() * sizeof(T));

                // Flush to make writes visible to GPU
                buffer->flush();

                VkBuffer vertexBuffers[1] = { *buffer };
                VkDeviceSize offsets[1] = { 0 };
                if (lineWidth > 0.0f)
                    vkCmdSetLineWidth(commandBuffer, lineWidth);
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);

                // Make sure that removed not in use by a command buffer.
                while (removeQueue.size() > MAX_FRAMES_IN_FLIGHT * 2) {
                    removeQueue.pop();
                }
            }

        private:
            std::unique_ptr<Buffer> buffer;
            uint32_t vertexCount{ 0 };
            std::queue<std::unique_ptr<Buffer>> removeQueue;
        };

        std::array<DrawBuffer<DebugRenderer::DrawVertex>, 2> lines;
        std::array<DrawBuffer<DebugRenderer::DrawVertex>, 2> thickLines;
        std::array<DrawBuffer<DebugRenderer::DrawVertex>, 2> triangles;
        std::array<DrawBuffer<DebugRenderer::DrawSpatialVertex>, 2> points;

        std::array<PipelineGraphics, 4> pipelines;
        PushHandler pushObject;
    };
}
