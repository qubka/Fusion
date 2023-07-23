#pragma once

#include "fusion/graphics/buffers/storage_buffer.h"
#include "fusion/graphics/pipelines/shader.h"

namespace fe {
    /**
     * @brief Class that handles a storage buffer.
     */
    class StorageHandler {
    public:
        explicit StorageHandler(bool multipipeline = false);
        explicit StorageHandler(const Shader::UniformBlock& uniformBlock, bool multipipeline = false);

        void push(void* data, size_t size) {
            if (this->size != size) {
                this->size = static_cast<uint32_t>(size);
                handlerStatus = Buffer::Status::Reset;
                return;
            }

            if (!uniformBlock || !storageBuffer)
                return;

            if (!bound) {
                storageBuffer->map();
                bound = true;
            }

            // If the buffer is already changed we can skip a memory comparison and just copy
            if (handlerStatus == Buffer::Status::Changed || storageBuffer->compare(data, size) != 0) {
                storageBuffer->copy(data, size);
                handlerStatus = Buffer::Status::Changed;
            }
        }

        template<typename T>
        void push(const T& object, size_t offset, size_t size) {
            if (!uniformBlock || !storageBuffer)
                return;

            if (!bound) {
                storageBuffer->unmap();
                bound = true;
            }

            // If the buffer is already changed we can skip a memory comparison and just copy
            if (handlerStatus == Buffer::Status::Changed || storageBuffer->compare(&object, size, offset) != 0) {
                storageBuffer->copy(&object, size, offset);
                handlerStatus = Buffer::Status::Changed;
            }
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

        const StorageBuffer* getStorageBuffer() const { return storageBuffer.get(); }

    private:
        std::optional<Shader::UniformBlock> uniformBlock;
        uint32_t size{ 0 };
        std::unique_ptr<StorageBuffer> storageBuffer;
        Buffer::Status handlerStatus;
        bool bound{ false };
        bool multipipeline;
    };
}
