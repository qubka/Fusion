#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class Buffer;
    class GridSubrender final : public Subrender {
    public:
        explicit GridSubrender(const Pipeline::Stage& pipelineStage);
        ~GridSubrender() override;

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        PushHandler pushObject;

        std::unique_ptr<Buffer> vertexBuffer;
    };
}
