#pragma once

#include "fusion/graphics/cameras/editor_camera.hpp"

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/buffers/uniform_handler.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class GridRenderer final : public Subrender {
    public:
        GridRenderer(const Pipeline::Stage& pipelineStage);
        ~GridRenderer();

    private:
        void render(const CommandBuffer& commandBuffer) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        //UniformHandler uniformObject;
        PushHandler pushObject;

        EditorCamera camera{};

        std::unique_ptr<Buffer> vertexBuffer;
    };
}
