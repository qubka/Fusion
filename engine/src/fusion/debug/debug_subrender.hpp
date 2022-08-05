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
        struct DrawBuffer {
            std::unique_ptr<Buffer> buffer;
            std::vector<T> vertices;

            void cmdRender(const CommandBuffer& commandBuffer, const std::vector<T>& newVertices, float lineWidth = 0.0f) {
                if (!newVertices.empty()) {
                    if (!buffer || vertices != newVertices) {
                        vertices = newVertices;
                        buffer = Buffer::StageToDeviceBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, sizeof(T) * vertices.size(), vertices.data());
                    }

                    VkBuffer vertexBuffers[1] = { *buffer };
                    VkDeviceSize offsets[1] = { 0 };
                    if (lineWidth > 0.0f)
                        vkCmdSetLineWidth(commandBuffer, lineWidth);
                    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
                    vkCmdDraw(commandBuffer, vertices.size(), 1, 0, 0);
                }
            }
        };

        std::array<DrawBuffer<DebugRenderer::DrawVertex>, 2> lines;
        std::array<DrawBuffer<DebugRenderer::DrawVertex>, 2> thickLines;
        std::array<DrawBuffer<DebugRenderer::DrawVertex>, 2> triangles;
        std::array<DrawBuffer<DebugRenderer::DrawSpatialVertex>, 2> points;
        std::array<PipelineGraphics, 4> pipelines;
        PushHandler pushObject;
    };
}
