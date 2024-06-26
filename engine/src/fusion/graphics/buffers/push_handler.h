#pragma once

#include "fusion/graphics/pipelines/shader.h"

namespace fe {
    class CommandBuffer;
    class Pipeline;

    /**
     * @brief Class that handles a pipeline push constant.
     */
    class FUSION_API PushHandler {
    public:
        explicit PushHandler(bool multipipeline = false);
        explicit PushHandler(const Shader::UniformBlock& uniformBlock, bool multipipeline = false);

        template<typename T>
        void push(const T& object, size_t offset, size_t size) {
            std::memcpy(data.get() + offset, &object, size);
        }

        template<typename T>
        void push(const std::string& uniformName, const T& object, size_t size = 0) {
            if (!uniformBlock)
                return;

            auto uniform = uniformBlock->getUniform(uniformName);
            if (!uniform)
                return;

            auto realSize = size;
            if (realSize == 0)
                realSize = glm::min(sizeof(object), static_cast<size_t>(uniform->getSize()));

            push(object, static_cast<size_t>(uniform->getOffset()), realSize);
        }

        bool update(const std::optional<Shader::UniformBlock>& uniformBlock);

        void bindPush(const CommandBuffer& commandBuffer, const Pipeline& pipeline);

    private:
        std::optional<Shader::UniformBlock> uniformBlock;
        std::unique_ptr<uint8_t[]> data;
        bool multipipeline;
    };
}
