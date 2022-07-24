#pragma once

#include "fusion/graphics/subrender.hpp"
#include "fusion/graphics/pipelines/pipeline.hpp"

namespace fe {
    /**
     * @brief Represents a system of post effects.
     */
    class PostPipeline : public Subrender {
    public:
        /**
         * Creates a new post pipeline.
         * @param pipelineStage The pipelines graphics stage.
         */
        explicit PostPipeline(const Pipeline::Stage& pipelineStage) : Subrender{pipelineStage} {}
        virtual ~PostPipeline() = default;
    };
}
