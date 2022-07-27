#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/buffers/uniform_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/storage_handler.hpp"

namespace fe {
    class LightSubrender final : public Subrender {
    public:
        explicit LightSubrender(const Pipeline::Stage& pipelineStage);
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