#pragma once

#include "fusion/graphics/subrender.h"
#include "fusion/graphics/pipelines/pipeline_graphics.h"
#include "fusion/graphics/buffers/uniform_handler.h"
#include "fusion/graphics/buffers/push_handler.h"
#include "fusion/graphics/descriptors/descriptors_handler.h"
#include "fusion/graphics/buffers/storage_handler.h"

namespace fe {
    class LightSubrender final : public Subrender {
    public:
        explicit LightSubrender(Pipeline::Stage pipelineStage);
        ~LightSubrender() override = default;

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        UniformHandler uniformObject;
        PushHandler pushObject;
    };
}