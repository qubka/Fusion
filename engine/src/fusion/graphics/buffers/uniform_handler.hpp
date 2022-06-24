#pragma once

#include "uniform_buffer.hpp"

#include "fusion/graphics/pipelines/shader.hpp"

namespace fe {
    /**
     * @brief Class that handles a uniform buffer.
     */
    class UniformHandler {
    public:
        explicit UniformHandler(bool multipipeline = false);
        explicit UniformHandler(const Shader::UniformBlock& uniformBlock, bool multipipeline = false);

        template<typename T>
        void push(const T& object, size_t offset, size_t size) {
            if (!uniformBlock || !uniformBuffer)
                return;

            if (!bound) {
                uniformBuffer->mapMemory(&this->data);
                bound = true;
            }

            // If the buffer is already changed we can skip a memory comparison and just copy.
            if (handlerStatus == Buffer::Status::Changed || std::memcmp(static_cast<char*>(this->data), &object, size) != 0) {
                memcpy(static_cast<char *>(this->data) + offset, &object, size);
                handlerStatus = Buffer::Status::Changed;
            }
        }

        template<typename T>
        void push(const std::string &uniformName, const T& object, size_t size = 0) {
            if (!uniformBlock || !uniformBuffer)
                return;

            auto uniform = uniformBlock->getUniform(uniformName);
            if (!uniform)
                return;

            auto realSize = size;
            if (realSize == 0)
                realSize = std::min(sizeof(object), static_cast<size_t>(uniform->getSize()));

            push(object, static_cast<size_t>(uniform->getOffset()), realSize);
        }

        bool update(const std::optional<Shader::UniformBlock>& uniformBlock);

        const UniformBuffer* getUniformBuffer() const { return uniformBuffer.get(); }

    private:
        std::optional<Shader::UniformBlock> uniformBlock;
        void *data = nullptr;
        uint32_t size = 0;
        std::unique_ptr<UniformBuffer> uniformBuffer;
        Buffer::Status handlerStatus;
        bool bound = false;
        bool multipipeline;
    };
}
