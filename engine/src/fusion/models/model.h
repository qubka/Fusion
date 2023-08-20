#pragma once

#include "fusion/models/mesh.h"
#include "fusion/assets/asset.h"
#include "fusion/graphics/pipelines/vertex.h"

class aiScene;
class aiNode;
class aiMesh;
class aiMaterial;

namespace fe {
    class Scene;
    class Texture2d;

    struct SceneObject {
        std::string name;
        glm::vec3 position{ 0.0f };
        glm::quat orientation{ quat::identity };
        glm::vec3 scale{ 1.0f };
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<SceneObject> children;

        SceneObject() = default;
        SceneObject(const char* name, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
                : name{name}, position{position}, orientation{orientation}, scale{scale} {
        }
    };

    class FUSION_API Model final : public Asset {
    public:
        // aiProcess_Triangulate by default
        Model() = default;
        explicit Model(uuids::uuid uuid, bool load = false);
        ~Model() override = default;

        uuids::uuid getUuid() const override { return uuid; }
        type_index getType() const override { return type_id<Model>; }
        const std::string& getName() const override { return root.name; }
        const fs::path& getPath() const override { return path; }
        bool isLoaded() const override { return loaded; }
        bool isInternal() const override { return internal; }

        const SceneObject& getRoot() const { return root; }
        const Mesh* getMesh(uint32_t index) const { return index < meshesLoaded.size() ? meshesLoaded[index] : nullptr; }

        operator bool() const { return !root.name.empty(); }

        void load() override { loadFromFile(); };
        void unload() override { };

    private:
        // TODO: May be load from shader ?
        static Vertex::Layout Layout;

        void loadFromFile();

        void processNode(const aiScene* scene, const aiNode* node, SceneObject& parent);
        void processMeshes(const aiScene* scene, const aiNode* node, SceneObject& parent);
        //void processLight(const aiScene* scene, const aiNode* node, const aiLight* light);
        //void processCamera(const aiScene* scene, const aiNode* node, const aiCamera* camera);

        //std::vector<std::shared_ptr<Texture2d>> loadTextures(const aiMaterial* material, int type);
        void appendVertex(std::vector<uint8_t>& outputBuffer, const aiScene* scene, const aiMesh* mesh, uint32_t vertexIndex);

        //static aiScene GenerateScene(const Mesh& mesh);

        SceneObject root;
        std::vector<const Mesh*> meshesLoaded;
        //std::unordered_map<fs::path, const Texture2d*> texturesLoaded;
        fs::path path;
        uuids::uuid uuid;
        bool loaded{ false };
        bool internal{ false };
    };
}
