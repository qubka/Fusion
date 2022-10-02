#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline_graphics.hpp"
#include "fusion/graphics/buffers/uniform_handler.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"
#include "fusion/graphics/descriptors/descriptors_handler.hpp"
#include "fusion/graphics/buffers/storage_handler.hpp"
#include "fusion/graphics/textures/texture2d.hpp"

namespace fe {
    class MeshSubrender final : public Subrender {
    public:
        explicit MeshSubrender(Pipeline::Stage pipelineStage);
        ~MeshSubrender() override = default;

    private:
        struct/* MEM_ALIGN*/ Light {
            glm::vec3 position{ 0.0f };
            float cutOff{ 0.0f };
            glm::vec3 direction{ 0.0f };
            float outerCutOff{ 0.0f };
            glm::vec3 ambient{ 0.0f };
            float constant{ 0.0f };
            glm::vec3 diffuse{ 0.0f };
            float linear{ 0.0f };
            glm::vec3 specular{ 0.0f };
            float quadratic{ 0.0f };
        };

        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        UniformHandler uniformObject;
        UniformHandler uniformScene;
        StorageHandler storageLights;

        PushHandler pushObject;

        fst::unordered_split_flatmap<const Descriptor*, float> bindlessDescriptors;
    };
}