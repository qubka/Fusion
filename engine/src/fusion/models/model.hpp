#pragma once

#include "fusion/graphics/pipelines/vertex.hpp"

class aiScene;
class aiNode;
class aiMesh;
class aiMaterial;

namespace fe {
    class Mesh;
    class Texture2d;
    class CommandBuffer;

    class Model {
    public:
        Model(const fs::path& filepath, const Vertex::Layout& layout = {{Vertex::Component::Position, Vertex::Component::Normal, Vertex::Component::RGBA}});
        ~Model() = default;

        void cmdRender(const CommandBuffer& commandBuffer) const;

        const glm::vec3& getMinExtents() const { return minExtents; }
        const glm::vec3& getMaxExtents() const { return maxExtents; }
        float getWidth() const { return maxExtents.x - minExtents.x; }
        float getHeight() const { return maxExtents.y - minExtents.y; }
        float getDepth() const { return maxExtents.z - minExtents.z; }
        float getRadius() const { return radius; }

    private:
        fs::path directory;
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<std::shared_ptr<Texture2d>> texturesLoaded;
        Vertex::Layout layout;

        glm::vec3 minExtents{ FLT_MAX };
        glm::vec3 maxExtents{ -FLT_MAX };
        glm::vec3 scale{ 1.0f };
        glm::vec3 center{ 0.0f };
        glm::vec2 uvScale{ 1.0f };
        float radius{ 0.0 };

        void processNode(const aiScene* scene, const aiNode* node);
        void processMesh(const aiScene* scene, const aiMesh* mesh);
        std::vector<std::shared_ptr<Texture2d>> loadTextures(const aiMaterial* material, int type);
        void appendVertex(std::vector<uint8_t>& outputBuffer, const aiScene* pScene, const aiMesh* mesh, uint32_t vertexIndex);

        //static aiScene GenerateScene(const Mesh& mesh);

        template<typename T>
        void appendOutput(std::vector<uint8_t>& outputBuffer, const T& t) {
            auto offset = outputBuffer.size();
            auto copySize = sizeof(T);
            outputBuffer.resize(offset + copySize);
            std::memcpy(outputBuffer.data() + offset, &t, copySize);
        }

        template<typename T>
        void appendOutput(std::vector<uint8_t>& outputBuffer, const std::vector<T>& v) {
            auto offset = outputBuffer.size();
            auto copySize = v.size() * sizeof(T);
            outputBuffer.resize(offset + copySize);
            std:: memcpy(outputBuffer.data() + offset, v.data(), copySize);
        }
    };
}
