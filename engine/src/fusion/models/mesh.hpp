#pragma once

#include <volk/volk.h>

#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"

namespace fe {
    class Mesh {
    public:
        Mesh() = default;
        template<typename T>
        explicit Mesh(const std::vector<T>& vertices) {
            setVertices(vertices);
        }
        template<typename V, typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
        explicit Mesh(const std::vector<V>& vertices, const std::vector<I>& indices) {
            setVertices(vertices);
            setIndices(indices);
        }
        ~Mesh() = default;

        bool cmdRender(const CommandBuffer& commandBuffer, uint32_t instances = 1) const;

        const Buffer* getVertexBuffer() const { return vertexBuffer.get(); }
        const Buffer* getIndexBuffer() const { return indexBuffer.get(); }
        uint32_t getVertexCount() const { return vertexCount; }
        uint32_t getIndexCount() const { return indexCount; }
        /*const glm::vec3& getMinExtents() const { return minExtents; }
        const glm::vec3& getMaxExtents() const { return maxExtents; }
        float getWidth() const { return maxExtents.x - minExtents.x; }
        float getHeight() const { return maxExtents.y - minExtents.y; }
        float getDepth() const { return maxExtents.z - minExtents.z; }
        float getRadius() const { return radius; }*/
        VkIndexType getIndexType() const { return indexType; }

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

    private:
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        uint32_t vertexCount{ 0 };
        uint32_t indexCount{ 0 };
        VkIndexType indexType{ VK_INDEX_TYPE_NONE_KHR };
        /*glm::vec3 minExtents{ FLT_MAX };
        glm::vec3 maxExtents{ -FLT_MAX };
        float radius{ 0.0 };*/
    };
}
