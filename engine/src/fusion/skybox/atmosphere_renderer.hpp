#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class Buffer;
    class AtmosphereRenderer final : public Subrender {
    public:
        explicit AtmosphereRenderer(const Pipeline::Stage& pipelineStage);
        ~AtmosphereRenderer() override;

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        PushHandler pushObject;

        std::unique_ptr<Buffer> vertexBuffer;
    };
}
