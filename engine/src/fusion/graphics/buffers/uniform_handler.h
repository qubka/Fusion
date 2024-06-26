#pragma once

#include "fusion/graphics/buffers/uniform_buffer.h"
#include "fusion/graphics/pipelines/shader.h"

namespace fe {
    /**
     * @brief Class that handles a uniform buffer.
     */
    class FUSION_API UniformHandler {
    public:
        explicit UniformHandler(bool multipipeline = false);
        explicit UniformHandler(const Shader::UniformBlock& uniformBlock, bool multipipeline = false);

        template<typename T>
        void push(const T& object, size_t offset, size_t size) {
            if (!uniformBlock || !uniformBuffer)
                return;

            if (!bound) {
                uniformBuffer->map();
                bound = true;
            }

            // If the buffer is already changed we can skip a memory comparison and just copy
            if (handlerStatus == Buffer::Status::Changed || uniformBuffer->compare(&object, size, offset) != 0) {
                uniformBuffer->copy(&object, size, offset);
                handlerStatus = Buffer::Status::Changed;
            }
        }

        template<typename T>
        void push(const std::string& uniformName, const T& object, size_t size = 0) {
            if (!uniformBlock || !uniformBuffer)
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

        const UniformBuffer* getUniformBuffer() const { return uniformBuffer.get(); }

    private:
        std::optional<Shader::UniformBlock> uniformBlock;
        uint32_t size{ 0 };
        std::unique_ptr<UniformBuffer> uniformBuffer;
        Buffer::Status handlerStatus;
        bool bound{ false };
        bool multipipeline;
    };
}
