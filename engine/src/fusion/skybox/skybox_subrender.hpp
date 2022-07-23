#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class Buffer;
    class TextureCube;
    class SkyboxSubrender final : public Subrender {
    public:
        explicit SkyboxSubrender(const Pipeline::Stage& stage);
        ~SkyboxSubrender() override;

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        PushHandler pushObject;

        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;

        std::unique_ptr<TextureCube> skyboxSampler;
    };
}
