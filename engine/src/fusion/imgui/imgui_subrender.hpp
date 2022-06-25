#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/images/image2d.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"

namespace fe {
    class ImguiSubrender : public Subrender {
    public:
        ImguiSubrender(const Pipeline::Stage& pipelineStage);
        ~ImguiSubrender() = default;

        void begin();

        void end();

        void render(const CommandBuffer& commandBuffer) override;

    private:
        bool updateBuffers();
        void setStyleColors();

        PipelineGraphics pipeline;

        std::unique_ptr<Image2d> font;
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        int32_t vertexCount{ 0 };
        int32_t indexCount{ 0 };

        DescriptorsHandler descriptorSet;
        PushHandler pushObject;
    };
}
