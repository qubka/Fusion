#pragma once

#include "fusion/models/mesh.hpp"
#include "fusion/assets/asset.hpp"
#include "fusion/graphics/pipelines/vertex.hpp"

class aiScene;
class aiNode;
class aiMesh;
class aiMaterial;

namespace fe {
    class Scene;
    class Texture2d;

    struct SceneObject {
        std::string name;
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::vector<std::shared_ptr<SceneObject>> children;
        glm::vec3 position;
        glm::quat oritentation;
        glm::vec3 scale;
    };

    class Model : public Asset {
    public:
        // aiProcess_Triangulate by default
        explicit Model(const fs::path& filepath, uint32_t defaultFlags = 0, const Vertex::Layout& layout = {{Vertex::Component::Position, Vertex::Component::Normal, Vertex::Component::Color}});

        type_index getType() const override { return type_id<Model>; }

        const std::shared_ptr<SceneObject>& getRoot() const { return root; }
        std::shared_ptr<Mesh> getMesh(const std::string& name) const {
            auto it = meshesLoaded.find(name);
            return it != meshesLoaded.end() ? it->second : nullptr;
        }

    private:
        void processNode(const aiScene* scene, const aiNode* node, std::shared_ptr<SceneObject>& parent);
        void processMeshes(const aiScene* scene, const aiNode* node, std::shared_ptr<SceneObject>& parent);
        //void processLight(const aiScene* scene, const aiNode* node, const aiLight* light);
        //void processCamera(const aiScene* scene, const aiNode* node, const aiCamera* camera);

        //std::vector<std::shared_ptr<Texture2d>> loadTextures(const aiMaterial* material, int type);
        void appendVertex(std::vector<std::byte>& outputBuffer, const aiScene* scene, const aiMesh* mesh, uint32_t vertexIndex);

        //static aiScene GenerateScene(const Mesh& mesh);

        template<typename T>
        void appendOutput(std::vector<std::byte>& outputBuffer, std::span<const T> t) {
            auto offset = outputBuffer.size();
            auto copySize = t.size() * sizeof(T);
            outputBuffer.resize(offset + copySize);
            std::memcpy(outputBuffer.data() + offset, t.data(), copySize);
        }

        fs::path directory;
        Vertex::Layout layout;

        std::shared_ptr<SceneObject> root;
        std::unordered_map<std::string, std::shared_ptr<Mesh>> meshesLoaded;
        //std::unordered_map<fs::path, std::shared_ptr<Texture2d>> texturesLoaded;
    };
}
