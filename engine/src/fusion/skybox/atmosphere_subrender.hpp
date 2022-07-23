#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class Buffer;
    class AtmosphereSubrender final : public Subrender {
    public:
        explicit AtmosphereSubrender(const Pipeline::Stage& stage);
        ~AtmosphereSubrender() override;

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        PushHandler pushObject;

        std::unique_ptr<Buffer> vertexBuffer;
    };
}
