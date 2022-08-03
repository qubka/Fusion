#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/uniform_handler.hpp"
#include "fusion/graphics/buffers/storage_handler.hpp"
#include "fusion/graphics/textures/texture2d.hpp"
#include "fusion/graphics/textures/texture_cube.hpp"
#include "fusion/utils/future.hpp"

namespace fe {
    class DeferredSubrender : public Subrender {
    public:
        explicit DeferredSubrender(Pipeline::Stage pipelineStage);
        ~DeferredSubrender() override = default;

        static std::unique_ptr<Texture2d> ComputeBRDF(uint32_t size);
        static std::unique_ptr<TextureCube> ComputeIrradiance(const std::shared_ptr<TextureCube>& source, uint32_t size);
        static std::unique_ptr<TextureCube> ComputePrefiltered(const std::shared_ptr<TextureCube>& source, uint32_t size);

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        struct DeferredLight {
            glm::vec4 color{ 0.0f, 0.0f, 0.0f, 1.0f };
            glm::vec3 position{ 0.0f };
            float radius{ 0.0f };
        };

        struct Fog {
            glm::vec4 color;
            float density;
            float gradient;
            float lowerLimit;
            float upperLimit;
        } fog;

        PipelineGraphics pipeline;

        DescriptorsHandler descriptorSet;
        UniformHandler uniformScene;
        StorageHandler storageLights;

        std::unique_ptr<Texture2d> brdf;

        std::shared_ptr<TextureCube> skybox;
        std::shared_ptr<Texture2d> test;

        std::unique_ptr<TextureCube> irradiance;
        std::unique_ptr<TextureCube> prefiltered;
    };
}
