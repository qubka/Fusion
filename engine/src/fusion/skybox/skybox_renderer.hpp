#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class Buffer;
    class ImageCube;
    class SkyboxRenderer final : public Subrender {
    public:
        explicit SkyboxRenderer(const Pipeline::Stage& pipelineStage);
        ~SkyboxRenderer() override;

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        PushHandler pushObject;

        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;

        std::unique_ptr<ImageCube> skyboxSampler;
    };
}
