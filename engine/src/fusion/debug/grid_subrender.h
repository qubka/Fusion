#pragma once

#include "fusion/graphics/subrender.h"
#include "fusion/graphics/pipelines/pipeline_graphics.h"
#include "fusion/graphics/descriptors/descriptors_handler.h"
#include "fusion/graphics/buffers/push_handler.h"

namespace fe {
    class GridSubrender final : public Subrender {
    public:
        explicit GridSubrender(Pipeline::Stage pipelineStage);
        ~GridSubrender() override;

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        PushHandler pushObject;
    };
}
