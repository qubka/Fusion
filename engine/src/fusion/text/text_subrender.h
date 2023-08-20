#pragma once

#include "fusion/graphics/subrender.h"
#include "fusion/graphics/pipelines/pipeline_graphics.h"
#include "fusion/graphics/buffers/uniform_handler.h"
#include "fusion/graphics/buffers/push_handler.h"
#include "fusion/graphics/descriptors/descriptors_handler.h"
#include "fusion/graphics/buffers/storage_handler.h"
#include "fusion/graphics/utils/indexed_draw_object.h"

namespace fe {
    struct TextVertex {
        glm::vec3 pos;
        uint32_t color;
        glm::vec3 uv;

        TextVertex(const glm::vec3& pos, uint32_t color, const glm::vec3& uv)
            : pos{pos}, color{color}, uv{uv} {
        }

        bool operator==(const TextVertex& rhs) const {
            return pos == rhs.pos && color == rhs.color && uv == rhs.uv;
        }

        bool operator!=(const TextVertex& rhs) const {
            return !operator==(rhs);
        }
    };

    class TextSubrender final : public Subrender {
    public:
        explicit TextSubrender(Pipeline::Stage pipelineStage);
        ~TextSubrender() override = default;

    private:
        void onUpdate() override {};
        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        PipelineGraphics pipeline;
        DescriptorsHandler descriptorSet;
        UniformHandler uniformObject;
        PushHandler pushObject;

        IndexedDrawObject<TextVertex, uint32_t> drawObject;
        std::vector<TextVertex> vertexBuffer;
        std::vector<uint32_t> indexBuffer;

        fst::unordered_split_flatmap<const Descriptor*, float> bindlessDescriptors;
    };
}