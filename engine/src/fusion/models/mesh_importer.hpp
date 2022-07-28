#pragma once

#include "mesh.hpp"

#include "fusion/graphics/pipelines/vertex.hpp"

#include <entt/entity/entity.hpp>

class aiScene;
class aiNode;
class aiMesh;
class aiMaterial;
class aiCamera;
class aiLight;

namespace fe {
    class Scene;
    class Texture2d;
    class MeshImporter {
    public:
       MeshImporter(const fs::path& filepath, const Vertex::Layout& layout = {{Vertex::Component::Position, Vertex::Component::Normal, Vertex::Component::Color}});

    private:
        void processNode(const aiScene* scene, const aiNode* node);
        void processMesh(const aiScene* scene, const aiNode* node, const aiMesh* mesh);
        void processLight(const aiScene* scene, const aiNode* node, const aiLight* light);
        void processCamera(const aiScene* scene, const aiNode* node, const aiCamera* camera);

        std::vector<std::shared_ptr<Texture2d>> loadTextures(const aiMaterial* material, int type);
        void appendVertex(std::vector<uint8_t>& outputBuffer, const aiScene* scene, const aiMesh* mesh, uint32_t vertexIndex);

        //static aiScene GenerateScene(const Mesh& mesh);

        /*template<typename T>
        void appendOutput(std::vector<uint8_t>& outputBuffer, const T& t) {
            auto offset = outputBuffer.size();
            auto copySize = sizeof(T);
            outputBuffer.resize(offset + copySize);
            std::memcpy(outputBuffer.data() + offset, &t, copySize);
        }*/

        template<typename T>
        void appendOutput(std::vector<uint8_t>& outputBuffer, std::span<const T> t) {
            auto offset = outputBuffer.size();
            auto copySize = t.size() * sizeof(T);
            outputBuffer.resize(offset + copySize);
            std::memcpy(outputBuffer.data() + offset, t.data(), copySize);
        }

        fs::path directory;

        std::unordered_map<const aiNode*, entt::entity> hierarchy;
        //std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<std::shared_ptr<Texture2d>> texturesLoaded;
        Vertex::Layout layout;
    };
}
