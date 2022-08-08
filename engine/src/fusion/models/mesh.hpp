#pragma once

#include "fusion/assets/asset.hpp"
#include "fusion/graphics/buffers/buffer.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/graphics/pipelines/vertex.hpp"
#include "fusion/geometry/aabb.hpp"

namespace fe {
    class Mesh : public Asset {
    public:
        template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
        explicit Mesh(const fs::path& filepath, const std::string& naming, const std::vector<std::byte>& vertices, const std::vector<T>& indices, const Vertex::Layout& layout) : layout{layout} {
            setVertices(vertices);
            setIndices(indices);
            path = filepath;
            name = naming;
        }
        /*template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
        explicit Mesh(fs::path path, std::string name, std::vector<std::byte>&& vertices, std::vector<T>&& indices, const Vertex::Layout& layout, float optimiseThreshold)
                : path{std::move(path)}
                , name{std::move(name)}
                , layout{layout} {
            size_t vertexCount = vertices.size() / layout.getStride();
            size_t indexCount = indices.size();
            size_t targetIndexCount = static_cast<size_t>(indices.size() * optimiseThreshold);

            float targetError = 1e-3f;
            float resultError;
            uint32_t options = 0;

            LOG_DEBUG << "Mesh Optimizer";
            LOG_DEBUG << "Before : " << indexCount << " indices " << vertexCount << " vertices";

            auto newIndexCount = meshopt_simplify<T>(
                    indices.data(),
                    indices.data(),
                    indices.size(),
                    (const float*)vertices.data(),
                    vertexCount,
                    layout.getStride(),
                    targetIndexCount,
                    targetError,
                    options,
                    &resultError
            );

            auto newVertexCount = meshopt_optimizeVertexFetch<T>( // return vertices (not vertex attribute values)
                    vertices.data(),
                    indices.data(),
                    newIndexCount, // total new indices (not faces)
                    vertices.data(),
                    vertexCount, // total vertices (not vertex attribute values)
                    layout.getStride() // vertex stride
            );

            LOG_DEBUG << "After : " << newIndexCount << " indices , " << newVertexCount << " vertices";

            setVertices(vertices);
            setIndices(indices);
        }*/
        ~Mesh() override = default;

        bool cmdRender(const CommandBuffer& commandBuffer, uint32_t instances = 1) const;

        const Buffer* getVertexBuffer() const { return vertexBuffer.get(); }
        const Buffer* getIndexBuffer() const { return indexBuffer.get(); }
        uint32_t getVertexCount() const { return vertexCount; }
        uint32_t getIndexCount() const { return indexCount; }
        VkIndexType getIndexType() const { return indexType; }
        const Vertex::Layout& getVertexLayout() const { return layout; }
        const AABB& getBoundingBox() const { return boundingBox; }

        std::vector<std::byte> getVertices() const {
            auto vertexStaging = Buffer::DeviceToStageBuffer(*vertexBuffer);

            vertexStaging->map();
            std::vector<std::byte> vertices(vertexCount * layout.getStride());
            vertexStaging->extract(vertices.data());
            vertexStaging->unmap();

            return vertices;
        }

        void setVertices(const std::vector<std::byte>& vertices) {
            vertexBuffer = nullptr;
            vertexCount = static_cast<uint32_t>(vertices.size() / layout.getStride());

            if (vertices.empty())
                return;

            vertexBuffer = Buffer::StageToDeviceBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, vertices.size(), vertices.data());

            glm::vec3 min{ FLT_MAX };
            glm::vec3 max{ -FLT_MAX };
            for (size_t i = 0; i < vertices.size(); i += layout.getStride()) {
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

        type_index getType() const override { return type_id<Mesh>; }

        void setMeshIndex(uint32_t index) { meshIndex = index; }
        uint32_t getMeshIndex() const { return meshIndex; }

    private:
        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;
        uint32_t vertexCount{ 0 };
        uint32_t indexCount{ 0 };
        uint32_t meshIndex{ 0 };
        VkIndexType indexType{ VK_INDEX_TYPE_NONE_KHR };
        AABB boundingBox;
        Vertex::Layout layout;
    };
}
