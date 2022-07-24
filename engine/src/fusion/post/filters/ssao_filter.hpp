#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/utils/future.hpp"
#include "fusion/graphics/buffers/uniform_handler.hpp"

namespace fe {
    class SsaoFilter : public PostFilter {
    public:
        explicit SsaoFilter(const Pipeline::Stage& pipelineStage);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

    private:
        std::vector<Shader::Define> getDefines() const;

        static std::shared_ptr<Texture2d> computeNoise(uint32_t size);

        UniformHandler uniformScene;

        Future<std::shared_ptr<Texture2d>> noise;
        std::vector<glm::vec3> kernel;
    };
}
