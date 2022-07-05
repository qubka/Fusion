#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/buffers/uniform_handler.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class GridRenderer final : public Subrender {
    public:
        explicit GridRenderer(const Pipeline::Stage& pipelineStage);
        ~GridRenderer();

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        //UniformHandler uniformObject;
        PushHandler pushObject;

        std::unique_ptr<Buffer> vertexBuffer;
    };
}
