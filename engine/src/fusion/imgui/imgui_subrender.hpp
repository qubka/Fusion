#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"

namespace fe {
    class ImguiSubrender : public Subrender {
    public:
        ImguiSubrender(const Pipeline::Stage& pipelineStage);
        ~ImguiSubrender() = default;

        void render(const CommandBuffer& commandBuffer) override;

    private:
        PipelineGraphics pipeline;
    };
}
