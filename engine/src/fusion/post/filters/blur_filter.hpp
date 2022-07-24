#pragma once

#include "fusion/post/post_filter.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"

namespace fe {
    class BlurFilter : public PostFilter {
    public:
        enum class Type {
            _5 = 5,
            _9 = 9,
            _13 = 13
        };

        explicit BlurFilter(const Pipeline::Stage& pipelineStage, const glm::vec2& direction, Type type = Type::_9);

        void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) override;

        const glm::vec2& getDirection() const { return direction; }
        void setDirection(const glm::vec2& direction) { this->direction = direction; }

    private:
        std::vector<Shader::Define> getDefines(const Type &type);

        PushHandler pushScene;

        Type type;
        glm::vec2 direction;
    };
}
