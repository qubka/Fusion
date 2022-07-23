#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"

namespace fe {
    class Model;
    class ModelSubrender final : public Subrender {
    public:
        explicit ModelSubrender(const Pipeline::Stage& stage);
        ~ModelSubrender() override;

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        PushHandler pushObject;

        std::unique_ptr<Model> cubeModel;
    };
}
