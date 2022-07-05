#pragma once

#include <volk.h>

#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"

namespace fe {
    class Mesh {
    public:
        Mesh() = default;
        template<typename T>
        explicit Mesh(const std::vector<T>& vertices, const std::vector<uint32_t>& indices = {}, VkIndexType indexType = VK_INDEX_TYPE_UINT32) : indexType{indexType} {
            setVertices(vertices);
            setIndices(indices);

            for (const auto& vertex : vertices) {
                minExtents = glm::min(vertex.position, minExtents);
                maxExtents = glm::max(vertex.position, maxExtents);
            }

            radius = std::max(glm::length(minExtents), glm::length(maxExtents));
        }
        ~Mesh() = default;

        bool cmdRender(const CommandBuffer& commandBuffer, uint32_t instances = 1);

        const Buffer* getVertexBuffer() const { return vertexBuffer.get(); }
        const Buffer* getIndexBuffer() const { return indexBuffer.get(); }
        uint32_t getVertexCount() const { return vertexCount; }
        uint32_t getIndexCount() const { return indexCount; }
        const glm::vec3& getMinExtents() const { return minExtents; }
        const glm::vec3& getMaxExtents() const { return maxExtents; }
        float getWidth() const { return maxExtents.x - minExtents.x; }
        float getHeight() const { return maxExtents.y - minExtents.y; }
        float getDepth() const { return maxExtents.z - minExtents.z; }
        float getRadius() const { return radius; }
        VkIndexType getIndexType() const { return indexType; }

        std::vector<uint32_t> getIndices();
        void setIndices(const std::vector<uint32_t>& indices);

        template<typename T>
        std::vector<T> getVertices() const {
            auto vertexStaging = Buffer::DeviceToStageBuffer(*vertexBuffer);

            vertexStaging->map();
            std::vector<T> vertices(vertexCount);
            vertexStaging->extract(vertices.data());
            vertexStaging->unmap();

            return vertices;
        }

        template<typename T>
        void setVertices(const std::vector<T>& vertices) {
            vertexBuffer = nullptr;
            vertexCount = static_cast<uint32_t>(vertices.size());

            if (vertices.empty())
                return;

            vertexBuffer = Buffer::StageToDeviceBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, sizeof(T) * vertices.size(), vertices.data());
        }

    private:
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        uint32_t vertexCount{ 0 };
        uint32_t indexCount{ 0 };
        VkIndexType indexType{ VK_INDEX_TYPE_UINT32 };
        glm::vec3 minExtents{ FLT_MAX };
        glm::vec3 maxExtents{ -FLT_MAX };
        float radius{ 0.0f };
    };
}
