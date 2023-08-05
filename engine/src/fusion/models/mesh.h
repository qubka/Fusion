#pragma once

#include "fusion/graphics/buffers/buffer.h"
#include "fusion/graphics/commands/command_buffer.h"
#include "fusion/geometry/aabb.h"

namespace fe {
    class FUSION_API Mesh {
        friend class Model;
    public:
        Mesh() = default;
        explicit Mesh(uint32_t index);
        ~Mesh() = default;

        bool cmdRender(const CommandBuffer& commandBuffer, uint32_t instances = 1) const;

        const Buffer* getVertexBuffer() const { return vertexBuffer.get(); }
        const Buffer* getIndexBuffer() const { return indexBuffer.get(); }
        uint32_t getVertexCount() const { return vertexCount; }
        uint32_t getIndexCount() const { return indexCount; }
        VkIndexType getIndexType() const { return indexType; }
        const AABB& getBoundingBox() const { return boundingBox; }

        std::vector<std::byte> getVertices(uint32_t stride) const {
            auto vertexStaging = Buffer::DeviceToStageBuffer(*vertexBuffer);

            vertexStaging->map();
            std::vector<std::byte> vertices(vertexCount * stride);
            vertexStaging->extract(vertices.data());
            vertexStaging->unmap();

            return vertices;
        }

        void setVertices(const std::vector<std::byte>& vertices, uint32_t stride) {
            vertexBuffer = nullptr;
            vertexCount = static_cast<uint32_t>(vertices.size() / stride);

            if (vertices.empty())
                return;

            vertexBuffer = Buffer::StageToDeviceBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, vertices.size(), vertices.data());

            glm::vec3 min{ FLT_MAX };
            glm::vec3 max{ -FLT_MAX };
            for (size_t i = 0; i < vertices.size(); i += stride) {
                const auto& position = *reinterpret_cast<const glm::vec3*>(&vertices[i]);
                min = glm::min(position, min);
                max = glm::max(position, max);
            }
            boundingBox = AABB{min, max};
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
        std::vector<T> getIndices() const {
            auto indexStaging = Buffer::DeviceToStageBuffer(*indexBuffer);

            indexStaging->map();
            std::vector<T> indices(indexCount);
            indexStaging->extract(indices.data());
            indexStaging->unmap();

            return indices;
        }

        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
        void setIndices(const std::vector<T>& indices) {
            indexBuffer = nullptr;
            indexCount = static_cast<T>(indices.size());

            if (indices.empty())
                return;

            indexBuffer = Buffer::StageToDeviceBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, sizeof(T) * indices.size(), indices.data());

            if (sizeof(uint8_t) == sizeof(T)) {
                indexType = VK_INDEX_TYPE_UINT8_EXT;
            } else if (sizeof(uint16_t) == sizeof(T)) {
                indexType = VK_INDEX_TYPE_UINT16;
            } else if (sizeof(uint32_t) == sizeof(T)) {
                indexType = VK_INDEX_TYPE_UINT32;
            } else {
                static_assert("Invalid type");
            }
        }

        uint32_t getIndex() const { return index; }

    private:
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        uint32_t vertexCount{ 0 };
        uint32_t indexCount{ 0 };
        VkIndexType indexType{ VK_INDEX_TYPE_NONE_KHR };
        uint32_t index{ UINT32_MAX };
        AABB boundingBox;
    };
}
