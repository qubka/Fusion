#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/buffers/uniform_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/storage_handler.hpp"

namespace fe {
    class MeshSubrender final : public Subrender {
    public:
        explicit MeshSubrender(Pipeline::Stage pipelineStage);
        ~MeshSubrender() override = default;

    private:
        struct PointLight {
            glm::vec4 color{ 0.0f, 0.0f, 0.0f, 1.0f };
            glm::vec3 position{ 0.0f };
            float radius{ 0.0f };
        };

        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        UniformHandler uniformObject;
        StorageHandler storageLights;
        PushHandler pushObject;
    };
}