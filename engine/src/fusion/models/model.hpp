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
        explicit Model(const fs::path& filepath, uint32_t defaultFlags = 0, const Vertex::Layout& layout = {{Vertex::Component::Position, Vertex::Component::Normal, Vertex::Component::Tangent, Vertex::Component::Bitangent, Vertex::Component::UV, Vertex::Component::UV}});
        //~Model() override;

        type_index getType() const override { return type_id<Model>; }

        const std::shared_ptr<SceneObject>& getRoot() const { return root; }
        std::shared_ptr<Mesh> getMesh(uint32_t index) const {
            for (const auto& mesh : meshesLoaded) {
                if (mesh->getMeshIndex() == index) {
                    return mesh;
                }
            }
            return nullptr;
        }

    private:
        void processNode(const aiScene* scene, const aiNode* node, std::shared_ptr<SceneObject>& parent);
        void processMeshes(const aiScene* scene, const aiNode* node, std::shared_ptr<SceneObject>& parent);
        //void processLight(const aiScene* scene, const aiNode* node, const aiLight* light);
        //void processCamera(const aiScene* scene, const aiNode* node, const aiCamera* camera);

        //std::vector<std::shared_ptr<Texture2d>> loadTextures(const aiMaterial* material, int type);
        void appendVertex(std::vector<std::byte>& outputBuffer, const aiScene* scene, const aiMesh* mesh, uint32_t vertexIndex);

        //static aiScene GenerateScene(const Mesh& mesh);

        fs::path directory;
        Vertex::Layout layout;

        std::shared_ptr<SceneObject> root;
        std::vector<std::shared_ptr<Mesh>> meshesLoaded;
        //std::unordered_map<fs::path, std::shared_ptr<Texture2d>> texturesLoaded;
    };
}
