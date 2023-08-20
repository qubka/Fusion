#pragma once

#include "debug_renderer.h"

#include "fusion/graphics/subrender.h"
#include "fusion/graphics/pipelines/pipeline_graphics.h"
#include "fusion/graphics/buffers/push_handler.h"
#include "fusion/graphics/utils/draw_object.h"

namespace fe {
    class DebugSubrender final : public Subrender {
    public:
        explicit DebugSubrender(Pipeline::Stage pipelineStage);
        ~DebugSubrender() override;

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        std::array<DrawObject<DebugRenderer::DrawVertex>, 2> lines;
        std::array<DrawObject<DebugRenderer::DrawVertex>, 2> thickLines;
        std::array<DrawObject<DebugRenderer::DrawVertex>, 2> triangles;
        std::array<DrawObject<DebugRenderer::DrawSpatialVertex>, 2> points;

        std::array<PipelineGraphics, 4> pipelines;
        PushHandler pushObject;
    };
}
